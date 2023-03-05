#pragma once
#include "core/Scene/Scene.h"
#include "Core/Application/Assert.h"
#include "Core/Scene/Components.h"
#include "Core/Application/UUID.h"

#include "entt.hpp"

namespace Clonemmings
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& entity) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ASSERT(!HasComponent<T>(), "Entity already has this compoent!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}
		template<typename T>
		T& GetComponent()
		{
			ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}
		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}
		template<typename T>
		void RemoveComponent()
		{
			ASSERT(!HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registary.remove<T>(m_EntityHandle);
		}
		//explict casting operators
		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}
		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

	private:
		entt::entity m_EntityHandle = { entt::null };
		Scene* m_Scene = nullptr;
	};
}