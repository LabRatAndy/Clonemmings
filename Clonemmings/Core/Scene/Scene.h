#pragma once
#include "Core/Application/TimeStep.h"
#include "Core/Application/UUID.h"
#include <entt.hpp>

class b2World;
namespace Clonemmings
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		void OnUpdateRuntime(TimeStep ts);
		void OnViewportResize(uint32_t width, uint32_t height);
		Entity GetPrimaryCameraEntity();

		void StartScene();
		void StopScene();


	private:
		entt::registry m_Registry;
		uint32_t m_Viewportwidth = 0;
		uint32_t m_Viewportheight = 0;
		b2World* m_PhysicsWorld = nullptr;
		bool m_IsRunning = false;
		bool m_IsPaused = false;

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
		void OnPhysicsStart();
		void OnPhysicsStop();
		friend class Entity;
		friend class SceneSerialiser;
	};
}
