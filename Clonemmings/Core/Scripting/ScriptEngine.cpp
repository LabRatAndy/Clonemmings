#include "Core/Scripting/ScriptEngine.h"
#include "Core/Scripting/ScriptGlue.h"

#include "Core/Application/Application.h"
#include "Core/Application/Utills.h"

#include<FileWatch.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/object.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>

namespace Clonemmings
{
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{"System.Single",ScriptFieldType::Float},
		{"System.Double",ScriptFieldType::Double},
		{"System.Boolean",ScriptFieldType::Bool},
		{"System.Char",ScriptFieldType::Char},
		{"System.Int16",ScriptFieldType::Short},
		{"System.Int32",ScriptFieldType::Int},
		{"System.Int64",ScriptFieldType::Long},
		{"System.Byte",ScriptFieldType::Byte},
		{"System.UInt16",ScriptFieldType::UShort},
		{"System.UInt32",ScriptFieldType::UInt},
		{"System.UInt64",ScriptFieldType::ULong},
		{"Clonemmings.Vector2",ScriptFieldType::Vector2},
		{"Clonemmings.Vector3",ScriptFieldType::Vector3},
		{"Clonemmings.Vector4",ScriptFieldType::Vector4},
		{"Clonemmings.Entity",ScriptFieldType::Entity},
	};

	namespace Utills
	{
		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblypath, bool loadPDB = false)
		{
			ScopedBuffer filedata = FileSystem::ReadBinaryFile(assemblypath);
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(filedata.As<char>(), filedata.Size(), 1, &status, 0);
			if (status != MONO_IMAGE_OK)
			{
				const char* errormessage = mono_image_strerror(status);
				return nullptr;
			}
			if (loadPDB)
			{
				std::filesystem::path pdbpath = assemblypath;
				pdbpath.replace_extension(".pdb");
				if (std::filesystem::exists(pdbpath))
				{
					ScopedBuffer pdbfiledata = FileSystem::ReadBinaryFile(pdbpath);
					mono_debug_open_image_from_memory(image, pdbfiledata.As<const mono_byte>(), pdbfiledata.Size());
					INFO("Loaded PDB {}", pdbpath);
				}
			}
			std::string pathstring = assemblypath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathstring.c_str(), &status, 0);
			mono_image_close(image);
			return assembly;
		}

		void PrintAssembleyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typedefinitiontable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numberoftypes = mono_table_info_get_rows(typedefinitiontable);

			for (int32_t i = 0; i < numberoftypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typedefinitiontable, i, cols, MONO_TYPEDEF_SIZE);
				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
				TRACE("{}.{}", nameSpace, name);
			}
		}

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* type)
		{
			std::string typeName = mono_type_get_name(type);
			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
			{
				LOGERROR("Unknown type : {}", typeName);
				return ScriptFieldType::None;
			}
			return it->second;
		}
	}
	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;
		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;
		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;
		std::filesystem::path CoreAssemblyFilepath;
		std::filesystem::path AppAssemblyFilepath;
		ScriptClass EntityClass;
		std::unordered_map<std::string, std::shared_ptr<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, std::shared_ptr<ScriptInstance>> EntityInstances;
		std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;
		std::unique_ptr <filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AssemblyReloadPending = false;
#ifdef DEBUG
		bool EnableDebugging = true;
#else
		bool EnableDebugging = false;
#endif
		Scene* SceneContext = nullptr;

	};

	static ScriptEngineData* s_Data = nullptr;

	static void OnAppAssemblyFilesystemEvent(const std::string& path, const filewatch::Event changetype)
	{
		if (!s_Data->AssemblyReloadPending && changetype == filewatch::Event::modified)
		{
			s_Data->AssemblyReloadPending = true;
			Application::Get().SubmitToMainThread([]()
				{
					s_Data->AppAssemblyFileWatcher.reset();
					ScriptEngine::ReLoadAssembly();
				});
		}
	}

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();
		InitMono();
		ScriptGlue::RegisterFunctions();
		bool status = LoadAssembly("Assets/Scripts/Script-Core.dll");
		if (!status)
		{
			LOGERROR("[ScriptEngine] could not load Script core assembly");
			return;
		}
		status = LoadAppAssembly("Assets/Scripts/Clonemmings.dll");
		if (!status)
		{
			LOGERROR("[ScriptEngine] could not load Clonemmings Script assembly");
			return;
		}
		LoadAssemblyClasses();
		ScriptGlue::RegisterComponents();

		s_Data->EntityClass = ScriptClass("Core", "Entity", true);
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		if (s_Data->EnableDebugging)
		{
			const char* argv[2] =
			{
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
				"--soft-breakpoints"
			};
			mono_jit_parse_options(2, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}
		MonoDomain* rootdomain = mono_jit_init("JITruntime");
		ASSERT(rootdomain);
		s_Data->RootDomain = rootdomain;
		if (s_Data->EnableDebugging)
		{
			mono_debug_domain_create(rootdomain);
		}
		mono_thread_set_main(mono_thread_current());
	}
	
	void ScriptEngine::ShutdownMono()
	{
		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;
		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}
	
	bool ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppDomain = mono_domain_create_appdomain("ScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);
		s_Data->CoreAssemblyFilepath = filepath;
		s_Data->CoreAssembly = Utills::LoadMonoAssembly(s_Data->CoreAssemblyFilepath, s_Data->EnableDebugging);
		if (s_Data->CoreAssembly == nullptr)
		{
			return false;
		}
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		return true;
	}

	bool ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppAssemblyFilepath = filepath;
		s_Data->AppAssembly = Utills::LoadMonoAssembly(s_Data->AppAssemblyFilepath, s_Data->EnableDebugging);
		if (s_Data->AppAssembly == nullptr)
		{
			return false;
		}
		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
		s_Data->AppAssemblyFileWatcher = std::make_unique<filewatch::FileWatch<std::string>>(s_Data->AppAssemblyFilepath.string(), OnAppAssemblyFilesystemEvent);
		s_Data->AssemblyReloadPending = false;
		return true;
	}

	void ScriptEngine::ReLoadAssembly()
	{
		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->AppDomain);
		LoadAssembly(s_Data->CoreAssemblyFilepath);
		LoadAppAssembly(s_Data->AppAssemblyFilepath);
		LoadAssemblyClasses();
		ScriptGlue::RegisterComponents();
		s_Data->EntityClass = ScriptClass("Core", "Entity", true);
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	bool ScriptEngine::EntityClassExists(const std::string& fullclassname)
	{
		return s_Data->EntityClasses.find(fullclassname) != s_Data->EntityClasses.end();
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptEngine::EntityClassExists(sc.ClassName))
		{
			UUID uuid = entity.GetUUID();
			std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
			s_Data->EntityInstances[uuid] = instance;

			if (s_Data->EntityScriptFields.find(uuid) != s_Data->EntityScriptFields.end())
			{
				const ScriptFieldMap& fieldmap = s_Data->EntityScriptFields.at(uuid);
				for (const auto& [name, fieldinstance] : fieldmap)
				{
					instance->SetFieldValueInternal(name, fieldinstance.m_Buffer);
				}
			}
			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, TimeStep ts)
	{
		UUID uuid = entity.GetUUID();
		if (s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end())
		{
			std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[uuid];
			instance->InvokeOnUpdate((float)ts);
		}
		else
		{
			LOGERROR("Could not find Scriptinstance for entity {}", uuid);
		}
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	std::shared_ptr<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityid)
	{
		auto it = s_Data->EntityInstances.find(entityid);
		if (it != s_Data->EntityInstances.end())
		{
			return nullptr;
		}
		return it->second;
	}

	std::shared_ptr<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
	{
		if (s_Data->EntityClasses.find(name) == s_Data->EntityClasses.end())
		{
			return nullptr;
		}
		return s_Data->EntityClasses.at(name);
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;
		s_Data->EntityInstances.clear();
	}

	std::unordered_map<std::string, std::shared_ptr<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
	}

	ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entity)
	{
		ASSERT(entity);
		UUID uuid = entity.GetUUID();
		return s_Data->EntityScriptFields[uuid];
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_Data->EntityClasses.clear();
		const MonoTableInfo* typedefinitionstable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numbertypes = mono_table_info_get_rows(typedefinitionstable);
		MonoClass* entityclass = mono_class_from_name(s_Data->CoreAssemblyImage, "Core", "Entity");
		for (int32_t i = 0; i < numbertypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typedefinitionstable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* classname = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullname;
			if (strlen(nameSpace) != 0)
			{
				fullname = fmt::format("{}.{}", nameSpace, classname);
			}
			else
			{
				fullname = classname;
			}
			MonoClass* monoclass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, classname);
			if (monoclass == entityclass)
			{
				continue;
			}
			bool isentity = mono_class_is_subclass_of(monoclass, entityclass, false);
			if (!isentity)
			{
				continue;
			}
			std::shared_ptr<ScriptClass> scriptclass = std::make_shared<ScriptClass>(nameSpace, classname);
			s_Data->EntityClasses[fullname] = scriptclass;
			// This routine is an iterator routine for retrieving the fields in a class.
			// You must pass a gpointer that points to zero and is treated as an opaque handle
			// to iterate over all of the elements. When no more values are available, the return value is NULL.
			int fieldcount = mono_class_num_fields(monoclass);
			WARN("{} has {} fields:", classname, fieldcount);
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoclass, &iterator))
			{
				const char* fieldname = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);
				if (flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType* monotype = mono_field_get_type(field);
					ScriptFieldType fieldtype = Utills::MonoTypeToScriptFieldType(monotype);
					WARN(" {} ({})", fieldname, Utills::ScriptFieldTypeToString(fieldtype));
					scriptclass->m_Fields[fieldname] = { fieldtype,fieldname,field };
				}
			}
		}
		auto& entityclasses = s_Data->EntityClasses;
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

	MonoObject* ScriptEngine::GetManagedInstance(UUID uuid)
	{
		ASSERT(s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end());
		return s_Data->EntityInstances.at(uuid)->GetManagedObject();
	}

	MonoObject* ScriptEngine::instantiateClass(MonoClass* monoclass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoclass);
		mono_runtime_object_init(instance);
		return instance;
	}

	ScriptClass::ScriptClass(const std::string& classnamespace, const std::string& classname, bool iscore) :m_ClassNameSpace(classnamespace), m_ClassName(classname)
	{
		m_MonoClass = mono_class_from_name(iscore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, m_ClassNameSpace.c_str(), m_ClassName.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::instantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int paramentercount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), paramentercount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* exception = nullptr;
		return mono_runtime_invoke(method, instance, params, &exception);
	}

	ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptclass, Entity entity) : m_ScriptClass(scriptclass)
	{
		m_Instance = m_ScriptClass->Instantiate();
		m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = s_Data->EntityClass.GetMethod("OnCreate", 0);
		m_OnUpdateMethod = s_Data->EntityClass.GetMethod("OnUpdate", 1);

		UUID uuid = entity.GetUUID();
		void* param = &uuid;
		m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_OnCreateMethod)
		{
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
		}
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (m_OnUpdateMethod)
		{
			void* params = &ts;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &params);
		}
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
		{
			return false;
		}
		const ScriptField& field = it->second;
		mono_field_get_value(m_Instance, field.ClassField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
		{
			return false;
		}
		const ScriptField& field = it->second;
		mono_field_set_value(m_Instance, field.ClassField, (void*)value);
	}
}