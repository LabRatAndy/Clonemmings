#include "Core/Scene/Scene.h"
#include "Core/Scene/Components.h"
#include "Core/Scene/Entity.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Application/Application.h"

namespace Clonemmings
{
	Scene::Scene()
	{

	}
	Scene::~Scene()
	{

	}
	Enitity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.Create(),this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}
	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}
	void Scene::OnUpdateRuntime(Timestep ts)
	{
		//rendering
		Camera* maincamera = nullptr;
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
				Application::Get()->GetRenderer()->SetCamera(maincamera, cameratransform);
				Application::Get()->GetRenderer()->StartBatch();
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriterendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					//rendering call to go here todo
				}
				Application::Get()->GetRenderer()->EndBatch();
			}
		}
	}
	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

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
			const auto& camera = view.GetComponent<CameraComponent>(entity);
			if (camera.Primary)
			{
				return { entity, this };
			}
		}
		return {};
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
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
		{
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		}
	}
	template<>
	void Scene::OnComponentAdded<SpriterendererComponent>(Entity entity, SpriterendererComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}
}