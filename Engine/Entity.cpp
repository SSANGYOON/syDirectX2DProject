#include "pch.h"
#include "Entity.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"

namespace SY {
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
	void Entity::PauseEntity()
	{
		if (HasComponent<Rigidbody2DComponent>())
		{
			auto& rb = GetComponent<Rigidbody2DComponent>();
			b2Body* body = (b2Body*)rb.RuntimeBody;

			if (body)
				body->SetEnabled(false);
		}
	}
	void Entity::ActivateEntity()
	{
		if (HasComponent<Rigidbody2DComponent>())
		{
			auto& rb = GetComponent<Rigidbody2DComponent>();
			b2Body* body = (b2Body*)rb.RuntimeBody;
			auto& tr = GetComponent<TransformComponent>();

			if (body) {
				if(rb.enabled)
					body->SetEnabled(true);
				body->SetTransform({ tr.translation.x,tr.translation.y }, tr.rotation.z);
			}
		}
	}
	void Entity::KillEntity()
	{
	}
}