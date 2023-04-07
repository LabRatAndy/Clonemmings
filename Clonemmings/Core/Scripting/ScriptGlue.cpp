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
	static void TransformComponent_GetTranslation(UUID uuid, glm::vec3* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		*outresult = entity.GetComponent<TransformComponent>().Translation;
	}
	static void TransformComponent_SetTranslation(UUID uuid, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<TransformComponent>().Translation = *translation;
	}
	static void TransformComponent_GetRotation(UUID uuid, glm::vec3* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		*outresult = entity.GetComponent<TransformComponent>().Rotation;
	}
	static void TransformComponent_SetRotation(UUID uuid, glm::vec3* rotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}
	static void TransformComponent_GetScale(UUID uuid, glm::vec3* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		*outresult = entity.GetComponent<TransformComponent>().Scale;
	}
	static void TransformComponent_SetScale(UUID uuid, glm::vec3* scale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<TransformComponent>().Scale = *scale;
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
	static ClonemmingComponent::ClonemingStatus ClonemmingComponent_GetStatus(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		return entity.GetComponent<ClonemmingComponent>().Status;
	}
	static void ClonemmingComponent_SetStatus(UUID uuid, ClonemmingComponent::ClonemingStatus status)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<ClonemmingComponent>().Status = status;
	}
	static uint32_t ClonemmingStartComponent_GetNumberOfClonemmings(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		return entity.GetComponent<ClonemmingStartComponent>().NumberOfClonemmings;
	}
	static void ClonemmingStartComponent_SetNumberOfClonemmings(UUID uuid, uint32_t numberofclonemmings)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<ClonemmingStartComponent>().NumberOfClonemmings = numberofclonemmings;
	}
	static uint32_t ClonemmingStartComponent_GetClonemmingReleaseRate(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		return entity.GetComponent<ClonemmingStartComponent>().ClonemmingReleaseRate;
	}
	static void ClonemmingStartComponent_SetClonemmingReleaseRate(UUID uuid, uint32_t releaserate)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<ClonemmingStartComponent>().ClonemmingReleaseRate = releaserate;
	}
	static uint32_t ClonemmingExitComponent_GetNumberOfClonemmings(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		return entity.GetComponent<ClonemmingExitComponent>().NumberOfClonemmings;
	}
	static void ClonemmingExitComponent_SetNumberOfClonemmings(UUID uuid, uint32_t numberofclonemming)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<ClonemmingExitComponent>().NumberOfClonemmings = numberofclonemming;
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
					managedtypename = fmt::format("Clonemming.{}", structname);
					managedtype = mono_reflection_type_from_name(managedtypename.data(), ScriptEngine::GetCoreAssemblyImage());
					if (!managedtype)
					{
						LOGERROR("Could not find component type: {}", managedtypename);
						return;
					}
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
		ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		ADD_INTERNAL_CALL(RigidBody2D_ApplyLinearImpulse);
		ADD_INTERNAL_CALL(RigidBody2D_ApplyLinearImpulseToCentre);
		ADD_INTERNAL_CALL(RigidBody2D_GetLinearVelocity);
		ADD_INTERNAL_CALL(RigidBody2D_GetType);
		ADD_INTERNAL_CALL(RigidBody2D_SetType);
		ADD_INTERNAL_CALL(Input_IsKeyDown);
		ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		ADD_INTERNAL_CALL(TransformComponent_GetScale);
		ADD_INTERNAL_CALL(TransformComponent_SetScale);
		ADD_INTERNAL_CALL(ClonemmingComponent_GetStatus);
		ADD_INTERNAL_CALL(ClonemmingComponent_SetStatus);
		ADD_INTERNAL_CALL(ClonemmingStartComponent_GetNumberOfClonemmings);
		ADD_INTERNAL_CALL(ClonemmingStartComponent_SetNumberOfClonemmings);
		ADD_INTERNAL_CALL(ClonemmingStartComponent_GetClonemmingReleaseRate);
		ADD_INTERNAL_CALL(ClonemmingStartComponent_SetClonemmingReleaseRate);
		ADD_INTERNAL_CALL(ClonemmingExitComponent_GetNumberOfClonemmings);
		ADD_INTERNAL_CALL(ClonemmingExitComponent_SetNumberOfClonemmings);

	}
}