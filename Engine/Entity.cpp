#include "pch.h"
#include "Entity.h"

namespace SY {
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}