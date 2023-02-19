#pragma once
#include "Core/Application/TimeStep.h"
#include "entt.hpp"

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

	private:
		entt::registry m_Registry;
		uint32_t m_Viewportwidth = 0;
		uint32_t m_Viewportheight = 0;

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		friend class Entity;
		friend class SceneSerialiser;
	};
}
