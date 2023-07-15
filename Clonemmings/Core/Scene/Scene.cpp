#include "Core/Scene/Scene.h"
#include "Core/Scene/CoreComponents.h"
#include "Core/Scene/Entity.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Application/Application.h"
#include "Core/Application/Physics2D.h"
#include "Core/Scripting/ScriptEngine.h"
#include "Core/Physic2D/PhysicsEngine.h"
#include "Core/Assets/AssetManager.h"

#include <box2d/b2_body.h>
#include <box2d/b2_world.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
namespace Clonemmings
{

	template<typename... component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttmap)
	{
		([&]()
			{
				auto view = src.view<component>();
				for (auto srcentity : view)
				{
					entt::entity dstentity = enttmap.at(src.get<IDComponent>(srcentity).ID);
					auto& srccomponent = src.get<component>(srcentity);
					dst.emplace_or_replace<component>(dstentity, srccomponent);
				}
			}(), ...);
	}
	template<typename... components>
	static void CopyComponent(ComponentGroup<components ...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttmap)
	{
		CopyComponent<components...>(dst, src, enttmap);
	}
	template<typename... component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
			{
				if (src.HasComponent<component>())
				{
					dst.AddOrReplaceComponent<component>(src.GetComponent<component>());
				}
			}(), ...);
	}
	template<typename... component>
	static void CopyComponentIfExists(ComponentGroup<component ...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<component...>(dst, src);
	}
	Scene::Scene()
	{

	}
	Scene::~Scene()
	{
		PhysicsEngine::Shutdown();
	}
	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}
	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(),this };
		uint32_t handle = entity;
		INFO("Created new entity name: {0}, ENTT handle: {1}", name, handle);
		auto& idc = entity.AddComponent<IDComponent>(uuid);
		INFO("Added ID component: uuid = {0}", idc.ID);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		INFO("Added Tag component: tag = {0}", tag.Tag);
		m_EntityMap[uuid] = entity;
		INFO("Entity added to entity map. Entity creation is complete!");
		return entity;
	}
	void Scene::DestroyEntity(Entity entity)
	{
		if (entity.HasComponent<RigidBody2DComponent>())
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			PhysicsEngine::DestroyBody((b2Body*)rb2d.RuntimeBody);
		}
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}
	Entity Scene::GetEntityByName(std::string_view name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tag = view.get<TagComponent>(entity);
			if (tag.Tag == name)
			{
				return Entity{ entity,this };
			}
		}
		return {};
	}
	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
		{
			return { m_EntityMap.at(uuid),this };
		}
		return {};
	}
	void Scene::OnUpdateRuntime(TimeStep ts)
	{
		if (!m_IsPaused && m_IsRunning)
		{
			//Physics
			{
				const int32_t velocityiterations = 6;
				const int32_t positioniterations = 2;
				PhysicsEngine::PhysicsWorldUpdate(ts);
				//retrive transforms from Box2D
				auto view = m_Registry.view<RigidBody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e,this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
					const auto& position = PhysicsEngine::GetPosition((b2Body*)rb2d.RuntimeBody);
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = PhysicsEngine::GetAngle((b2Body*)rb2d.RuntimeBody);
					
				}
			}
			//scripting
			{
				auto view = m_Registry.view<ScriptComponent>();
				for (auto e : view)
				{
					Entity entity = { e,this };
					ScriptEngine::OnUpdateEntity(entity, ts);
				}
			}
			//scripting
			{
				auto view = m_Registry.view<ScriptComponent>();
				for (auto e : view)
				{
					Entity entity = { e,this };
					ScriptEngine::OnUpdateEntity(entity, ts);
				}
			}
		}
		

		//rendering
		SceneCamera* maincamera = nullptr;
		glm::mat4 cameratransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
				if (camera.Primary)
				{
					maincamera = &camera.Camera;
					cameratransform = transform.GetTransform();
					break;
				}
			}
		}
		{
			if (maincamera)
			{
				Application::Get().GetRenderer().SetCamera(maincamera, cameratransform);
				Application::Get().GetRenderer().StartBatch(BatchType::Quad);
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					glm::vec2 size = { transform.Scale.x,transform.Scale.y };
					std::shared_ptr<Texture> texture = Application::GetAssetManager().GetAssetAs<Texture>(sprite.TextureAssetHandle);
					Application::Get().GetRenderer().DrawBatchedRotatedQuad(transform.Translation, size, texture, sprite.Colour, glm::degrees(transform.Rotation.z), sprite.TilingFactor, (int)entity);
				}
				Application::Get().GetRenderer().EndBatch(BatchType::Quad);
				Application::Get().GetRenderer().StartBatch(BatchType::Line);
				auto view = m_Registry.view<RectangleComponent>();
				for (auto entity : view)
				{
					auto rectangle = view.get<RectangleComponent>(entity);
					Application::Get().GetRenderer().SetLineWidth(rectangle.LineThickness);
					Application::Get().GetRenderer().DrawRectangle(rectangle.GetTransform(), rectangle.Colour, (int)entity);
				}
				Application::Get().GetRenderer().EndBatch(BatchType::Line);
			}
		}
	}
	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_Viewportwidth = width;
		m_Viewportheight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameracomponent = view.get<CameraComponent>(entity);
			if (!cameracomponent.FixedAspectRatio)
			{
				cameracomponent.Camera.SetViewportSize(width, height, false);
			}
		}
	}
	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
			{
				return { entity, this };
			}
		}
		return {};
	}
	void Scene::OnPhysicsStart()
	{
		PhysicsEngine::Initialise(glm::vec2(0.0, -9.81));
		
		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e,this };
			SetUpPhysicOnEntity(entity);
		}
	}
	void Scene::OnPhysicsStop()
	{
		PhysicsEngine::Shutdown();
	}
	void Scene::SetGameLayer(Layer* gamelayer)
	{
		ASSERT(gamelayer);
		m_GameLayer = gamelayer;
	}
	Layer* Scene::GetGameLayer()
	{
		return m_GameLayer;
	}
	void Scene::StartScene()
	{
		m_IsRunning = true;
		OnPhysicsStart();
		ScriptEngine::OnRuntimeStart(this);
		auto view = m_Registry.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e,this };
			ScriptEngine::OnCreateEntity(entity);
		}
	}
	void Scene::StopScene()
	{
		m_IsRunning = false;
		OnPhysicsStop();
		ScriptEngine::OnRuntimeStop();
	}
	std::shared_ptr<Scene> Scene::Copy(std::shared_ptr<Scene> other)
	{
		std::shared_ptr<Scene> newscene = std::make_shared<Scene>();
		newscene->m_Viewportheight = other->m_Viewportheight;
		newscene->m_Viewportwidth = other->m_Viewportwidth;
		newscene->m_GameLayer = other->m_GameLayer;

		auto& srcreg = other->m_Registry;
		auto& dstreg = newscene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttmap;

		//copying entities to new scene along with uuid and tags
		auto srcentities = srcreg.view<IDComponent>();
		for (auto e : srcentities)
		{
			UUID uuid = srcreg.get<IDComponent>(e).ID;
			const std::string& name = srcreg.get<TagComponent>(e).Tag;
			Entity entity = newscene->CreateEntityWithUUID(uuid, name);
			enttmap[uuid] = (entt::entity)entity;
		}

		//copy rest of the components
		CopyComponent(AllComponents{}, dstreg, srcreg, enttmap);

		return newscene;
	}
	Entity Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newentity = CreateEntity(name);
		CopyComponentIfExists(AllComponents{}, newentity, entity);
		return newentity;
	}
	void Scene::SetUpPhysicOnEntity(Entity entity)
	{
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		rb2d.RuntimeBody = PhysicsEngine::CreateBody(rb2d, transform);

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			PhysicsEngine::CreateBoxCollider(bc2d, transform, (b2Body*)rb2d.RuntimeBody);
		}
		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
			PhysicsEngine::CreateCircleCollider(cc2d, transform, (b2Body*)rb2d.RuntimeBody);
		}
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_Viewportwidth > 0 && m_Viewportheight > 0)
		{
			component.Camera.SetViewportSize(m_Viewportwidth, m_Viewportheight, false);
		}
	}
	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<ClonemmingComponent>(Entity entity, ClonemmingComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<ClonemmingStartComponent>(Entity entity, ClonemmingStartComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<ClonemmingExitComponent>(Entity entity, ClonemmingExitComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<RectangleComponent>(Entity entity, RectangleComponent& component)
	{

	}
}