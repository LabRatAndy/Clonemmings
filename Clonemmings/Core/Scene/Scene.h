#pragma once
#include "Core/Application/TimeStep.h"
#include "Core/Application/UUID.h"
#include "Core/Application/Layer.h"
#include "Game/GameLevelData.h"
#include <entt.hpp>
#include <glm/glm.hpp>

class b2World;
namespace Clonemmings
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();

		static std::shared_ptr<Scene> Copy(std::shared_ptr<Scene> otherscene);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		void OnUpdateRuntime(TimeStep ts);
		void OnViewportResize(uint32_t width, uint32_t height);
		Entity GetPrimaryCameraEntity();
		Entity GetEntityByName(std::string_view name);
		Entity GetEntityByUUID(UUID uuid);
		Entity DuplicateEntity(Entity entity);
		void SplitHorizontalEntity(Entity entity, const glm::vec3& gapposition, float gapwidth);
		void SplitVerticalEntity(Entity entity, const glm::vec3& gapposition, float gapheight);

		void SetGameLayer(Layer* gamelayer); 
		Layer* GetGameLayer(); 
		
		void StartScene();
		void StopScene();
		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused; }
		void SetPaused(bool paused) { m_IsPaused = paused; }
		void SetUpPhysicOnEntity(Entity entity);
		
		GameLevelData& GetGameLevelData() { return m_GameLevelData; }
		void SetGameLevelData(const GameLevelData& gameleveldata) { m_GameLevelData = gameleveldata; }


	private:
		entt::registry m_Registry;
		uint32_t m_Viewportwidth = 0;
		uint32_t m_Viewportheight = 0;
		b2World* m_PhysicsWorld = nullptr;
		bool m_IsRunning = false;
		bool m_IsPaused = true;
		Layer* m_GameLayer = nullptr;
		GameLevelData m_GameLevelData;

		std::unordered_map<UUID, Entity> m_EntityMap;

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
		void OnPhysicsStart();
		void OnPhysicsStop();
		friend class Entity;
		friend class SceneSerialiser;
	};
}
