#include "Core/Scene/Scene.h"
#include "Core/Scene/Components.h"
#include "Core/Scene/Entity.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Application/Application.h"
#include "Core/Application/Physics2D.h"

#include <box2d/b2_body.h>
#include <box2d/b2_world.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
namespace Clonemmings
{
	Scene::Scene()
	{

	}
	Scene::~Scene()
	{

	}
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(),this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}
	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}
	void Scene::OnUpdateRuntime(TimeStep ts)
	{
		if (!m_IsPaused)
		{
			//Physics
			{
				const int32_t velocityiterations = 6;
				const int32_t positioniterations = 2;
				m_PhysicsWorld->Step(ts, velocityiterations, positioniterations);
				//retrive transforms from Box2D
				auto view = m_Registry.view<RigidBody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e,this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
					b2Body* body = (b2Body*)rb2d.RuntimeBody;
					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
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
				Application::Get().GetRenderer().StartBatch();
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					//rendering call to go here todo
				}
				Application::Get().GetRenderer().EndBatch();
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
				cameracomponent.Camera.SetViewportSize(width, height);
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
		m_PhysicsWorld = new b2World({ 0.0f,-9.8f });
		
		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e,this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2BodyDef bodydef;
			bodydef.type = Utills::Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodydef.position.Set(transform.Translation.x, transform.Translation.y);
			bodydef.angle = transform.Rotation.z;
			b2Body* body = m_PhysicsWorld->CreateBody(&bodydef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
				b2PolygonShape boxshape;
				boxshape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);
				b2FixtureDef fixturedef;
				fixturedef.shape = &boxshape;
				fixturedef.density = bc2d.Density;
				fixturedef.friction = bc2d.Friction;
				fixturedef.restitution = bc2d.Restitution;
				fixturedef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixturedef);
			}
			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
				b2CircleShape circleshape;
				circleshape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleshape.m_radius = transform.Scale.x * cc2d.Radius;
				b2FixtureDef fixturedef;
				fixturedef.shape = &circleshape;
				fixturedef.density = cc2d.Density;
				fixturedef.friction = cc2d.Friction;
				fixturedef.restitution = cc2d.Restitution;
				fixturedef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixturedef);
			}
		}
	}
	void Scene::OnPhysicsStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}
	void Scene::StartScene()
	{
		m_IsRunning = true;
		OnPhysicsStart();
	}
	void Scene::StopScene()
	{
		m_IsRunning = false;
		OnPhysicsStop();
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
			component.Camera.SetViewportSize(m_Viewportwidth, m_Viewportheight);
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
}