#include "Core/Scene/Scene.h"
#include "Core/Scene/CoreComponents.h"
#include "Core/Scene/Entity.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Application/Application.h"
#include "Core/Application/Physics2D.h"
#include "Core/Scripting/ScriptEngine.h"
#include "Core/Physic2D/PhysicsEngine.h"

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
			PhysicsEngine::RemovePhysicsEntity(entity.GetUUID());
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
					const auto& position = PhysicsEngine::GetPosition(entity.GetUUID());
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = PhysicsEngine::GetAngle(entity.GetUUID());
					
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
					Application::Get().GetRenderer().DrawBatchedRotatedQuad(transform.Translation, size, sprite.Tex, sprite.Colour, glm::degrees(transform.Rotation.z), sprite.TilingFactor, (int)entity);
				}
				Application::Get().GetRenderer().EndBatch(BatchType::Quad);
				Application::Get().GetRenderer().StartBatch(BatchType::Line);
				auto view = m_Registry.view<RectangleComponent>();
				for (auto entity : view)
				{
					auto& rectangle = view.get<RectangleComponent>(entity);
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
		auto view2 = m_Registry.view<ClonemmingComponent, ScriptComponent>();
		for (auto e : view2)
		{
			auto uuid = m_Registry.get<IDComponent>(e).ID;
			auto instance = ScriptEngine::GetEntityScriptInstance(uuid);
			instance->SetFieldValue("m_MaxSurvivableVelocityChange", m_GameLevelData.DeadVelocityChange);
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
		if (entity.HasComponent<ClonemmingComponent>())
			PhysicsEngine::AddClonemming(entity);
		else
		{
			if (entity.HasComponent<RigidBody2DComponent>())
				PhysicsEngine::AddPhysicsEntity(entity);
		}

	}
	void Scene::SplitHorizontalEntity(Entity entity, const glm::vec3& gapposition, float gapwidth)
	{
		// step 1 workout the 4 vertices of the entity in world space 
		const auto& tc = entity.GetComponent<TransformComponent>();
		glm::vec4 entitypos[4]; //BL, BR, TR, TL
		for (size_t n = 0; n < 4; n++)
		{
			entitypos[n] = tc.GetTransform() * Application::Get().GetRenderer().GetQuadVertices((uint32_t)n);
		}
		//step 2 work out top 2 new vertices for gap and step 3 work out bottom 2 new vertices for gap
		glm::vec4 entitygappos[4]; // BL, BR, TR, TL
		float leftx = gapposition.x - gapwidth / 2;
		float rightx = gapposition.x + gapwidth / 2;
		//gap BL
		entitygappos[0] = { leftx, entitypos[0].y, 0.0f,1.0f };
		//gap BR
		entitygappos[1] = { rightx,entitypos[0].y,0.0f,1.0f };
		//gap TR
		entitygappos[2] = { rightx,entitypos[2].y,0.0f,1.0f };
		//gap TL
		entitygappos[3] = { leftx,entitypos[2].y,0.0f,1.0f };
		//step 5 work out size of the new entities for scale
		glm::vec3 size1; //left hand entity
		glm::vec3 size2; //right hand entity
		size1.x = glm::abs(entitypos[0].x - entitygappos[0].x);
		size1.y = tc.Scale.y;
		size1.z = tc.Scale.z;
		size2.x = glm::abs(entitygappos[1].x - entitypos[1].x);
		size2.y = tc.Scale.y;
		size2.z = tc.Scale.z;
		//step 4 work out centres of new entities for translations
		glm::vec3 centre1; //left hand entity
		glm::vec3 centre2; //right hand entity
		centre1.x = entitypos[0].x + size1.x / 2;
		centre1.y = entitypos[0].y + size1.y / 2;
		centre1.z = 0.0f;
		centre2.x = entitygappos[1].x + size2.x / 2;
		centre2.y = entitygappos[1].y + size2.y / 2;
		centre2.z = 0.0f;
		//step 6 create 2 new entities with same setting as original apart form tranform
		const auto& tag = entity.GetComponent<TagComponent>();
		std::string name = tag.Tag;
		name.append(" Left hand part!");
		Entity lhsentity = CreateEntity(name);
		auto& lhstc = lhsentity.GetComponent<TransformComponent>();
		lhstc.Scale = size1;
		lhstc.Translation = centre1;
		lhstc.Rotation = glm::vec3(0.0f);
		name.clear();
		name = tag.Tag;
		name.append(" Right hand part!");
		Entity rhsentity = CreateEntity(name);
		auto& rhstc = rhsentity.GetComponent<TransformComponent>();
		rhstc.Scale = size2;
		rhstc.Translation = centre2;
		rhstc.Rotation = glm::vec3(0.0f);
		if (entity.HasComponent<RigidBody2DComponent>())
		{
			const auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			auto& lhsrb2d = lhsentity.AddComponent<RigidBody2DComponent>();
			auto& rhsrb2d = rhsentity.AddComponent<RigidBody2DComponent>();
			lhsrb2d.FixedRotation = rb2d.FixedRotation;
			rhsrb2d.FixedRotation = rb2d.FixedRotation;
			lhsrb2d.RuntimeBody = rb2d.RuntimeBody;
			rhsrb2d.RuntimeBody = rb2d.RuntimeBody;
			lhsrb2d.Type = rb2d.Type;
			rhsrb2d.Type = rb2d.Type;
		}
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			const auto& src = entity.GetComponent<SpriteRendererComponent>();
			auto& lhssrc = lhsentity.AddComponent<SpriteRendererComponent>();
			auto& rhssrc = rhsentity.AddComponent<SpriteRendererComponent>();
			lhssrc.Colour = src.Colour;
			lhssrc.Tex = src.Tex;
			lhssrc.TilingFactor = src.TilingFactor;
			rhssrc.Colour = src.Colour;
			rhssrc.Tex = src.Tex;
			rhssrc.TilingFactor = src.TilingFactor;
		}
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			const auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			auto& lhsbc2d = lhsentity.AddComponent<BoxCollider2DComponent>();
			auto& rhsbc2d = rhsentity.AddComponent<BoxCollider2DComponent>();
			lhsbc2d.Category = bc2d.Category;
			lhsbc2d.Density = bc2d.Density;
			lhsbc2d.Friction = bc2d.Friction;
			lhsbc2d.Mask = bc2d.Mask;
			lhsbc2d.Offset = bc2d.Offset;
			lhsbc2d.Restitution = bc2d.Restitution;
			lhsbc2d.RestitutionThreshold = bc2d.RestitutionThreshold;
			lhsbc2d.RuntimeFixture = bc2d.RuntimeFixture;
			lhsbc2d.Size = bc2d.Size;
			rhsbc2d.Category = bc2d.Category;
			rhsbc2d.Density = bc2d.Density;
			rhsbc2d.Friction = bc2d.Friction;
			rhsbc2d.Mask = bc2d.Mask;
			rhsbc2d.Offset = bc2d.Offset;
			rhsbc2d.Restitution = bc2d.Restitution;
			rhsbc2d.RestitutionThreshold = bc2d.RestitutionThreshold;
			rhsbc2d.RuntimeFixture = bc2d.RuntimeFixture;
			rhsbc2d.Size = bc2d.Size;
		}
		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			const auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
			auto& lhscc2d = lhsentity.AddComponent<CircleCollider2DComponent>();
			auto& rhscc2d = rhsentity.AddComponent<CircleCollider2DComponent>();
			lhscc2d.Category = cc2d.Category;
			lhscc2d.Density = cc2d.Density;
			lhscc2d.Friction = cc2d.Friction;
			lhscc2d.Mask = cc2d.Mask;
			lhscc2d.Offset = cc2d.Offset;
			lhscc2d.Restitution = cc2d.Restitution;
			lhscc2d.RestitutionThreshold = cc2d.RestitutionThreshold;
			lhscc2d.RuntimeFixture = cc2d.RuntimeFixture;
			lhscc2d.Radius = cc2d.Radius;
			rhscc2d.Category = cc2d.Category;
			rhscc2d.Density = cc2d.Density;
			rhscc2d.Friction = cc2d.Friction;
			rhscc2d.Mask = cc2d.Mask;
			rhscc2d.Offset = cc2d.Offset;
			rhscc2d.Restitution = cc2d.Restitution;
			rhscc2d.RestitutionThreshold = cc2d.RestitutionThreshold;
			rhscc2d.RuntimeFixture = cc2d.RuntimeFixture;
			rhscc2d.Radius = cc2d.Radius;
		}
		PhysicsEngine::AddPhysicsEntity(lhsentity);
		PhysicsEngine::AddPhysicsEntity(rhsentity);
	}
	void Scene::SplitVerticalEntity(Entity entity, const glm::vec3& gapposition, float gapheight)
	{
		const auto& tc = entity.GetComponent<TransformComponent>();
		glm::vec4 entitypos[4]; //BL, BR, TR, TL
		for (size_t n = 0; n < 4; n++)
		{
			entitypos[n] = tc.GetTransform() * Application::Get().GetRenderer().GetQuadVertices((uint32_t)n);
		}
		glm::vec4 entitygappos[4]; // BL, BR, TR, TL
		float topy = gapposition.y + gapheight / 2;
		float basey = gapposition.y - gapheight / 2;
		entitygappos[0] = { entitypos[0].x,basey,0.0f,1.0f };
		entitygappos[1] = { entitypos[1].x,basey,0.0f,1.0f };
		entitygappos[2] = { entitypos[2].x,topy,0.0f,1.0f };
		entitygappos[3] = { entitypos[3].x,topy,0.0f,1.0f };
		glm::vec3 size1;	//top
		glm::vec3 size2;	//bottom
		size1.x = tc.Scale.x;
		size1.y = glm::abs(entitypos[3].y - entitygappos[3].y);
		size1.z = tc.Scale.z;
		size2.x = tc.Scale.x;
		size2.y = glm::abs(entitygappos[0].y - entitypos[0].y);
		size2.z = tc.Scale.z;
		glm::vec3 centre1;
		glm::vec3 centre2;
		centre1.x = entitypos[3].x + size1.x / 2;
		centre1.y = entitypos[3].y - size1.y / 2;
		centre1.z = 0.0f;
		centre2.x = entitypos[0].x + size2.x / 2;
		centre2.y = entitypos[0].y + size2.y / 2;
		centre2.z = 0.0f;
		const auto& tag = entity.GetComponent<TagComponent>();
		std::string name = tag.Tag;
		name.append(" top part!");
		Entity lhsentity = CreateEntity(name);
		auto& lhstc = lhsentity.GetComponent<TransformComponent>();
		lhstc.Scale = size1;
		lhstc.Translation = centre1;
		lhstc.Rotation = glm::vec3(0.0f);
		name.clear();
		name = tag.Tag;
		name.append(" bottom part!");
		Entity rhsentity = CreateEntity(name);
		auto& rhstc = rhsentity.GetComponent<TransformComponent>();
		rhstc.Scale = size2;
		rhstc.Translation = centre2;
		rhstc.Rotation = glm::vec3(0.0f);
		if (entity.HasComponent<RigidBody2DComponent>())
		{
			const auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			auto& lhsrb2d = lhsentity.AddComponent<RigidBody2DComponent>();
			auto& rhsrb2d = rhsentity.AddComponent<RigidBody2DComponent>();
			lhsrb2d.FixedRotation = rb2d.FixedRotation;
			rhsrb2d.FixedRotation = rb2d.FixedRotation;
			lhsrb2d.RuntimeBody = rb2d.RuntimeBody;
			rhsrb2d.RuntimeBody = rb2d.RuntimeBody;
			lhsrb2d.Type = rb2d.Type;
			rhsrb2d.Type = rb2d.Type;
		}
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			const auto& src = entity.GetComponent<SpriteRendererComponent>();
			auto& lhssrc = lhsentity.AddComponent<SpriteRendererComponent>();
			auto& rhssrc = rhsentity.AddComponent<SpriteRendererComponent>();
			lhssrc.Colour = src.Colour;
			lhssrc.Tex = src.Tex;
			lhssrc.TilingFactor = src.TilingFactor;
			rhssrc.Colour = src.Colour;
			rhssrc.Tex = src.Tex;
			rhssrc.TilingFactor = src.TilingFactor;
		}
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			const auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			auto& lhsbc2d = lhsentity.AddComponent<BoxCollider2DComponent>();
			auto& rhsbc2d = rhsentity.AddComponent<BoxCollider2DComponent>();
			lhsbc2d.Category = bc2d.Category;
			lhsbc2d.Density = bc2d.Density;
			lhsbc2d.Friction = bc2d.Friction;
			lhsbc2d.Mask = bc2d.Mask;
			lhsbc2d.Offset = bc2d.Offset;
			lhsbc2d.Restitution = bc2d.Restitution;
			lhsbc2d.RestitutionThreshold = bc2d.RestitutionThreshold;
			lhsbc2d.RuntimeFixture = bc2d.RuntimeFixture;
			lhsbc2d.Size = bc2d.Size;
			rhsbc2d.Category = bc2d.Category;
			rhsbc2d.Density = bc2d.Density;
			rhsbc2d.Friction = bc2d.Friction;
			rhsbc2d.Mask = bc2d.Mask;
			rhsbc2d.Offset = bc2d.Offset;
			rhsbc2d.Restitution = bc2d.Restitution;
			rhsbc2d.RestitutionThreshold = bc2d.RestitutionThreshold;
			rhsbc2d.RuntimeFixture = bc2d.RuntimeFixture;
			rhsbc2d.Size = bc2d.Size;
		}
		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			const auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
			auto& lhscc2d = lhsentity.AddComponent<CircleCollider2DComponent>();
			auto& rhscc2d = rhsentity.AddComponent<CircleCollider2DComponent>();
			lhscc2d.Category = cc2d.Category;
			lhscc2d.Density = cc2d.Density;
			lhscc2d.Friction = cc2d.Friction;
			lhscc2d.Mask = cc2d.Mask;
			lhscc2d.Offset = cc2d.Offset;
			lhscc2d.Restitution = cc2d.Restitution;
			lhscc2d.RestitutionThreshold = cc2d.RestitutionThreshold;
			lhscc2d.RuntimeFixture = cc2d.RuntimeFixture;
			lhscc2d.Radius = cc2d.Radius;
			rhscc2d.Category = cc2d.Category;
			rhscc2d.Density = cc2d.Density;
			rhscc2d.Friction = cc2d.Friction;
			rhscc2d.Mask = cc2d.Mask;
			rhscc2d.Offset = cc2d.Offset;
			rhscc2d.Restitution = cc2d.Restitution;
			rhscc2d.RestitutionThreshold = cc2d.RestitutionThreshold;
			rhscc2d.RuntimeFixture = cc2d.RuntimeFixture;
			rhscc2d.Radius = cc2d.Radius;
		}
		PhysicsEngine::AddPhysicsEntity(lhsentity);
		PhysicsEngine::AddPhysicsEntity(rhsentity);
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