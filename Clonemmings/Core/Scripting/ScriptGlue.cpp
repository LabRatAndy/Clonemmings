#include "Core/Scripting/ScriptGlue.h"
#include "Core/Scripting/ScriptEngine.h"

#include "Core/Application/UUID.h"
#include "Core/Application/KeyMouseCodes.h"
#include "Core/Application/Input.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "Core/Application/Physics2D.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <box2d/b2_body.h>

#include <iostream>

namespace Clonemmings
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define ADD_INTERNAL_CALL(name) mono_add_internal_call("Clonemmings.InternalCalls::"#name, name)

	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << " , " << parameter << std::endl;
	}
	static void Vector3_Normalise(glm::vec3* parameter, glm::vec3* outresult)
	{
		*outresult = glm::normalize(*parameter);
	}
	static float Vector3_Dot(glm::vec3* parameter)
	{
		return glm::dot(*parameter, *parameter);
	}
	static MonoObject* GetInstance(UUID entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}
	static bool Entity_HasComponent(UUID uuid, MonoReflectionType* componenttype)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		MonoType* managedtype = mono_reflection_type_get_type(componenttype);
		ASSERT(s_EntityHasComponentFuncs.find(managedtype) != s_EntityHasComponentFuncs.end());
		return s_EntityHasComponentFuncs.at(managedtype)(entity);
	}
	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* namecStr = mono_string_to_utf8(name);
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByName(namecStr);
		mono_free(namecStr);
		if (!entity)
		{
			return 0;
		}
		return entity.GetUUID();
	}
	static void Transform_GetTranslation(UUID uuid, glm::vec3* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		*outresult = entity.GetComponent<TransformComponent>().Translation;
	}
	static void Transform_SetTranslation(UUID uuid, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<TransformComponent>().Translation = *translation;
	}
	static void RigidBody2D_ApplyLinearImpulse(UUID uuid, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}
	static void RigidBody2D_ApplyLinearImpulseToCentre(UUID uuid, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}
	static void RigidBody2D_GetLinearVelocity(UUID uuid, glm::vec2* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		const b2Vec2& linearvelocity = body->GetLinearVelocity();
		*outresult = glm::vec2(linearvelocity.x, linearvelocity.y);
	}
	static RigidBody2DComponent::BodyType RigidBody2D_GetType(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		return Utills::Rigidbody2DTypeFromBox2DBody(body->GetType());
	}
	static void RigidBody2D_SetType(UUID uuid, RigidBody2DComponent::BodyType bodytype)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->SetType(Utills::Rigidbody2DTypeToBox2DBody(bodytype));
	}
	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}
	template<typename... component>
	static void RegisterComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structname = typeName.substr(pos + 1);
				std::string managedtypename = fmt::format("Core.{}", structname);
				MonoType* managedtype = mono_reflection_type_from_name(managedtypename.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedtype)
				{
					LOGERROR("Could not find component type: {}", managedtypename);
					return;
				}
				s_EntityHasComponentFuncs[managedtype] = [](Entity entity) {return entity.HasComponent<component>(); };
			}(), ...);
	}
	template<typename... component>
	static void RegisterComponent(ComponentGroup<component...>)
	{
		RegisterComponent<component...>();
	}
	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}
	void ScriptGlue::RegisterFunctions()
	{
		ADD_INTERNAL_CALL(NativeLog);
		ADD_INTERNAL_CALL(Vector3_Normalise);
		ADD_INTERNAL_CALL(Vector3_Dot);
		ADD_INTERNAL_CALL(Transform_GetTranslation);
		ADD_INTERNAL_CALL(Transform_SetTranslation);
		ADD_INTERNAL_CALL(RigidBody2D_ApplyLinearImpulse);
		ADD_INTERNAL_CALL(RigidBody2D_ApplyLinearImpulseToCentre);
		ADD_INTERNAL_CALL(RigidBody2D_GetLinearVelocity);
		ADD_INTERNAL_CALL(RigidBody2D_GetType);
		ADD_INTERNAL_CALL(RigidBody2D_SetType);
		ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
}