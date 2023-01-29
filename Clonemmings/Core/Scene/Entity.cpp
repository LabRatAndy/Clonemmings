#include "Core/Scene/Entity.h"

namespace Clonemmings
{
	Entity::Entity(entt::entity entityhandle, Scene* scene) : m_EntityHandle(entityhandle), m_Scene(scene)
	{

	}
}