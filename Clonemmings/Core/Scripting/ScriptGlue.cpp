#include "Core/Scripting/ScriptGlue.h"
#include "Core/Scripting/ScriptEngine.h"

#include "Core/Application/UUID.h"
#include "Core/Application/KeyMouseCodes.h"
#include "Core/Application/Input.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "Core/Application/Application.h"
#include "Game/GameLayer.h"
#include "Core/Physic2D/PhysicsEngine.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>


#include <iostream>

namespace Clonemmings
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_EntityAddComponentFuncs;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_EntityRemoveComponentFuncs;

#define ADD_INTERNAL_CALL(name) mono_add_internal_call("Core.InternalCalls::"#name, name)

	static void Native_Log(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		WARN(str);
	}
#pragma region Vector3 functions
	static void Vector3_Normalise(glm::vec3* parameter, glm::vec3* outresult)
	{
		*outresult = glm::normalize(*parameter);
	}
	static float Vector3_Dot(glm::vec3* parameter)
	{
		return glm::dot(*parameter, *parameter);
	}
#pragma endregion
	static MonoObject* GetInstance(UUID entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}
#pragma region Entity  
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
	static void Entity_AddComponent(UUID uuid, MonoReflectionType* componenttype)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		MonoType* managedtype = mono_reflection_type_get_type(componenttype);
		ASSERT(s_EntityAddComponentFuncs.find(managedtype) != s_EntityAddComponentFuncs.end());
		s_EntityAddComponentFuncs.at(managedtype)(entity);		
	}
	static void Entity_RemoveComponent(UUID uuid, MonoReflectionType* componenttype)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		MonoType* managedtype = mono_reflection_type_get_type(componenttype);
		ASSERT(s_EntityRemoveComponentFuncs.find(managedtype) != s_EntityRemoveComponentFuncs.end());
		s_EntityRemoveComponentFuncs.at(managedtype)(entity);
	}
	static uint64_t Entity_CreateNewEntity(MonoString* name)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		char* cStr = mono_string_to_utf8(name);
		std::string entityname(cStr);
		mono_free(cStr);
		Entity entity = scene->CreateEntity(entityname);
		ScriptEngine::OnCreateEntity(entity);
		return entity.GetUUID();
	}
	static uint64_t Entity_CreateNewClonemming(MonoString* name)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity clonemming = scene->CreateEntity();
		if (name != nullptr)
		{
			char* cStr = mono_string_to_utf8(name);
			std::string clonemmingname(cStr);
			mono_free(cStr);
			clonemming.GetComponent<TagComponent>().Tag = clonemmingname;
		}
		//clonemming component
		auto& cc = clonemming.AddComponent<ClonemmingComponent>();
		auto& src = clonemming.AddComponent<SpriteRendererComponent>();
		src.Colour = { 0.100689799, 0.729729772, 0.0957946256, 1.0 };
		src.TilingFactor = 1.0f;
		src.TextureAssetHandle = 0;
		auto& rb2d = clonemming.AddComponent<RigidBody2DComponent>();
		rb2d.Type = RigidBody2DComponent::BodyType::Dynamic;
		rb2d.FixedRotation = false;
		auto& bc2d = clonemming.AddComponent<BoxCollider2DComponent>();
		bc2d.Offset = { 0.0,0.0 };
		bc2d.Size = { 0.5,0.5 };
		bc2d.Density = 1;
		bc2d.Friction = 0.5;
		bc2d.Restitution = 0;
		bc2d.RestitutionThreshold = 0.5;
		auto& sc = clonemming.AddComponent<ScriptComponent>();
		sc.ClassName = "Clonemmings.Clonemming";
		ScriptEngine::OnCreateEntity(clonemming);
		WARN("Creatated clonemming uuid: {0}", clonemming.GetUUID());
		return clonemming.GetUUID();
	}
	static void Entity_SetupPhysics(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		scene->SetUpPhysicOnEntity(entity);
	}
	static void Entity_DestroyEntity(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		scene->DestroyEntity(entity);
	}
	static bool Entity_IsSelectedEntity(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		GameLayer* gamelayer = (GameLayer*)(scene->GetGameLayer());
		ASSERT(gamelayer);
		Entity& selectedentity = gamelayer->GetSelectedEntity();
		if (!selectedentity)
		{
			return false;
		}
		Entity& entity = scene->GetEntityByUUID(uuid);
		if (selectedentity == entity)
		{
			return true;
		}
		return false;

	}
	static void Entity_AddRectangleComponent(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		ASSERT(!entity.HasComponent<RectangleComponent>(), "Entity already has rectangle component");
		entity.AddComponent<RectangleComponent>();
	}
	static void Entity_RemoveRectangleComponent(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		ASSERT(entity.HasComponent<RectangleComponent>(), "Entity doesn't have rectangle component");
		entity.RemoveComponent<RectangleComponent>();
	}
#pragma endregion
#pragma region RectangleComponent
	static void RectangleComponent_GetTranslation(UUID uuid, glm::vec3* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		*outresult = entity.GetComponent<RectangleComponent>().Translation;
	}
	static void RectangleComponent_SetTranslation(UUID uuid, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<RectangleComponent>().Translation = *translation;
	}
	static void RectangleComponent_GetRotation(UUID uuid, glm::vec3* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		*outresult = entity.GetComponent<RectangleComponent>().Rotation;
	}
	static void RectangleComponent_SetRotation(UUID uuid, glm::vec3* rotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<RectangleComponent>().Rotation = *rotation;
	}
	static void RectangleComponent_GetScale(UUID uuid, glm::vec3* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		*outresult = entity.GetComponent<RectangleComponent>().Scale;
	}
	static void RectangleComponent_SetScale(UUID uuid, glm::vec3* scale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<RectangleComponent>().Scale = *scale;
	}
	static void RectangleComponent_GetColour(UUID uuid, glm::vec4* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		*outresult = entity.GetComponent<RectangleComponent>().Colour;
	}
	static void RectangleComponent_SetColour(UUID uuid, glm::vec4* colour)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<RectangleComponent>().Colour = *colour;
	}
	static float Rectangle_Component_GetLineThickness(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		return entity.GetComponent<RectangleComponent>().LineThickness;
	}
	static void Rectangle_Component_SetLineThickness(UUID uuid, float linethickness)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<RectangleComponent>().LineThickness = linethickness;
	}
#pragma endregion

#pragma region TransformComponent 


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
#pragma endregion
#pragma region RigidBody2DComponent
	static void RigidBody2DComponent_ApplyLinearImpulse(UUID uuid, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		PhysicsEngine::ApplyLinearImpulse((b2Body*)rb2d.RuntimeBody, *impulse, *point, wake);
	}
	static void RigidBody2DComponent_ApplyLinearImpulseToCentre(UUID uuid, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		PhysicsEngine::ApplyLinearImpulseToCentre((b2Body*)rb2d.RuntimeBody, *impulse, wake);
	}
	static void RigidBody2DComponent_GetLinearVelocity(UUID uuid, glm::vec2* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		*outresult = PhysicsEngine::GetLinearVelocity((b2Body*)rb2d.RuntimeBody);
	}
	static RigidBody2DComponent::BodyType RigidBody2DComponent_GetType(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		return PhysicsEngine::GetBodyType((b2Body*)rb2d.RuntimeBody);
	}
	static void RigidBody2DComponent_SetType(UUID uuid, RigidBody2DComponent::BodyType bodytype)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		PhysicsEngine::SetBodyType((b2Body*)rb2d.RuntimeBody, bodytype);
	}
	static float RigidBody2DComponent_GetMass(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		return PhysicsEngine::GetMass((b2Body*)rb2d.RuntimeBody);
	}
	static void RigidBody2DComponent_ApplyForce(UUID uuid, glm::vec2* force, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		PhysicsEngine::ApplyForce((b2Body*)rb2d.RuntimeBody, *force, *point, wake);
	}
	static void RigidBody2DComponent_ApplyForceTocentre(UUID uuid, glm::vec2* force, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		PhysicsEngine::ApplyForceToCentre((b2Body*)rb2d.RuntimeBody, *force, wake);
	}
	static void RigidBody2DComponent_ApplyTorque(UUID uuid, float torque, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		PhysicsEngine::ApplyTorque((b2Body*)rb2d.RuntimeBody, torque, wake);
	}
	static void RigidBody2DComponent_ApplyAngularImpulse(UUID uuid, float impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		PhysicsEngine::ApplyAngularImpulse((b2Body*)rb2d.RuntimeBody, impulse, wake);
	}
	static bool RigidBody2DComponent_HasContact(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		return PhysicsEngine::IsContact((b2Body*)rb2d.RuntimeBody);
	}
	static bool RigidBody2DComponent_HasContactTop(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		ASSERT(entity.HasComponent<BoxCollider2DComponent>());
		auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
		return PhysicsEngine::IsContactTop((b2Body*)rb2d.RuntimeBody, bc2d.Size);
	}
	static bool RigidBody2DComponent_HasContactBottom(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		ASSERT(entity.HasComponent<BoxCollider2DComponent>());
		auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
		return PhysicsEngine::IsContactBottom((b2Body*)rb2d.RuntimeBody, bc2d.Size);
	}
	static bool RigidBody2DComponent_HasContactLeft(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		ASSERT(entity.HasComponent<BoxCollider2DComponent>());
		auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
		return PhysicsEngine::IsContactLeft((b2Body*)rb2d.RuntimeBody, bc2d.Size);
	}
	static bool RigidBody2DComponent_HasContactRight(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		ASSERT(entity.HasComponent<BoxCollider2DComponent>());
		auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
		return PhysicsEngine::IsContactRight((b2Body*)rb2d.RuntimeBody, bc2d.Size);
	}
#pragma endregion
#pragma region SpriteRendererComponent
	static void SpriteRendererComponent_GetColour(UUID uuid, glm::vec4* outresult)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity  entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		*outresult = entity.GetComponent<SpriteRendererComponent>().Colour;
	}
	static void SpriteRendererComponent_SetColour(UUID uuid, glm::vec4* colour)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<SpriteRendererComponent>().Colour = *colour;
	}
	static float SpriteRendererComponent_GetTilingFactor(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		return entity.GetComponent<SpriteRendererComponent>().TilingFactor;
	}
	static void SpriteRendererComponent_SetTilingFactor(UUID uuid, float tilingfactor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<SpriteRendererComponent>().TilingFactor = tilingfactor;
	}
#pragma endregion
	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}
	// game component stuff
#pragma region ClonemmingComponent
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
	static float ClonemmingComponent_GetWalkSpeed(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		return entity.GetComponent<ClonemmingComponent>().WalkSpeed;
	}
	static void ClonemmingComponent_SetWalkSpeed(UUID uuid, float walkspeed)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<ClonemmingComponent>().WalkSpeed = walkspeed;
	}
	static float ClonemmingComponent_GetDigSpeed(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		return entity.GetComponent<ClonemmingComponent>().DigSpeed;
	}
	static void ClonemmingComponent_SetDigSpeed(UUID uuid, float digspeed)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<ClonemmingComponent>().DigSpeed = digspeed;
	}
	static float ClonemmingComponent_GetMineSpeed(UUID uuid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		return entity.GetComponent<ClonemmingComponent>().MineSpeed;
	}
	static void ClonemmingComponent_SetMineSpeed(UUID uuid, float minespeed)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(uuid);
		ASSERT(entity);
		entity.GetComponent<ClonemmingComponent>().MineSpeed = minespeed;
	}
#pragma endregion
#pragma region ClonemmingStartComponent
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
#pragma endregion
#pragma region ClonemmingExitComponent
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
#pragma endregion
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
					managedtypename = fmt::format("Clonemmings.{}", structname);
					managedtype = mono_reflection_type_from_name(managedtypename.data(), ScriptEngine::GetCoreAssemblyImage());
					if (!managedtype)
					{
						LOGERROR("Could not find component type: {}", managedtypename);
						return;
					}
				}
				s_EntityHasComponentFuncs[managedtype] = [](Entity entity) {return entity.HasComponent<component>(); };
				s_EntityAddComponentFuncs[managedtype] = [](Entity entity) {entity.AddComponent<component>(); };
				s_EntityRemoveComponentFuncs[managedtype] = [](Entity entity) {entity.RemoveComponent<component>(); };
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
		ADD_INTERNAL_CALL(Native_Log);
		ADD_INTERNAL_CALL(Vector3_Normalise);
		ADD_INTERNAL_CALL(Vector3_Dot);
		ADD_INTERNAL_CALL(Entity_CreateNewClonemming);
		ADD_INTERNAL_CALL(Entity_CreateNewEntity);
		ADD_INTERNAL_CALL(Entity_HasComponent);
		ADD_INTERNAL_CALL(Entity_FindEntityByName);
		ADD_INTERNAL_CALL(Entity_SetupPhysics);
		ADD_INTERNAL_CALL(Entity_DestroyEntity);
		ADD_INTERNAL_CALL(Entity_IsSelectedEntity);
		ADD_INTERNAL_CALL(Entity_AddRectangleComponent);
		ADD_INTERNAL_CALL(Entity_RemoveRectangleComponent);
		ADD_INTERNAL_CALL(Entity_AddComponent);
		ADD_INTERNAL_CALL(Entity_RemoveComponent);
		ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		ADD_INTERNAL_CALL(TransformComponent_GetScale);
		ADD_INTERNAL_CALL(TransformComponent_SetScale);
		ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulse);
		ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulseToCentre);
		ADD_INTERNAL_CALL(RigidBody2DComponent_GetLinearVelocity);
		ADD_INTERNAL_CALL(RigidBody2DComponent_GetType);
		ADD_INTERNAL_CALL(RigidBody2DComponent_SetType);
		ADD_INTERNAL_CALL(RigidBody2DComponent_GetMass);
		ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyAngularImpulse);
		ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyForce);
		ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyForceTocentre);
		ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyTorque);
		ADD_INTERNAL_CALL(RigidBody2DComponent_HasContact);
		ADD_INTERNAL_CALL(RigidBody2DComponent_HasContactLeft);
		ADD_INTERNAL_CALL(RigidBody2DComponent_HasContactRight);
		ADD_INTERNAL_CALL(RigidBody2DComponent_HasContactTop);
		ADD_INTERNAL_CALL(RigidBody2DComponent_HasContactBottom);
		ADD_INTERNAL_CALL(SpriteRendererComponent_GetColour);
		ADD_INTERNAL_CALL(SpriteRendererComponent_SetColour);
		ADD_INTERNAL_CALL(SpriteRendererComponent_GetTilingFactor);
		ADD_INTERNAL_CALL(SpriteRendererComponent_SetTilingFactor);
		ADD_INTERNAL_CALL(RectangleComponent_GetTranslation);
		ADD_INTERNAL_CALL(RectangleComponent_SetTranslation);
		ADD_INTERNAL_CALL(RectangleComponent_GetRotation);
		ADD_INTERNAL_CALL(RectangleComponent_SetRotation);
		ADD_INTERNAL_CALL(RectangleComponent_GetScale);
		ADD_INTERNAL_CALL(RectangleComponent_SetScale);
		ADD_INTERNAL_CALL(RectangleComponent_GetColour);
		ADD_INTERNAL_CALL(RectangleComponent_SetColour);
		ADD_INTERNAL_CALL(Rectangle_Component_GetLineThickness);
		ADD_INTERNAL_CALL(Rectangle_Component_SetLineThickness);
		ADD_INTERNAL_CALL(Input_IsKeyDown);
		ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		ADD_INTERNAL_CALL(TransformComponent_GetScale);
		ADD_INTERNAL_CALL(TransformComponent_SetScale);
		ADD_INTERNAL_CALL(ClonemmingComponent_GetStatus);
		ADD_INTERNAL_CALL(ClonemmingComponent_SetStatus);
		ADD_INTERNAL_CALL(ClonemmingComponent_GetWalkSpeed);
		ADD_INTERNAL_CALL(ClonemmingComponent_SetWalkSpeed);
		ADD_INTERNAL_CALL(ClonemmingComponent_GetDigSpeed);
		ADD_INTERNAL_CALL(ClonemmingComponent_SetDigSpeed);
		ADD_INTERNAL_CALL(ClonemmingComponent_GetMineSpeed);
		ADD_INTERNAL_CALL(ClonemmingComponent_SetMineSpeed);
		ADD_INTERNAL_CALL(ClonemmingStartComponent_GetNumberOfClonemmings);
		ADD_INTERNAL_CALL(ClonemmingStartComponent_SetNumberOfClonemmings);
		ADD_INTERNAL_CALL(ClonemmingStartComponent_GetClonemmingReleaseRate);
		ADD_INTERNAL_CALL(ClonemmingStartComponent_SetClonemmingReleaseRate);
		ADD_INTERNAL_CALL(ClonemmingExitComponent_GetNumberOfClonemmings);
		ADD_INTERNAL_CALL(ClonemmingExitComponent_SetNumberOfClonemmings);
		ADD_INTERNAL_CALL(GetInstance);
	}
}