#pragma once
#include "Component.h"

#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_world_callbacks.h"
namespace SY {

	namespace Utils {

		inline b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
		{
			switch (bodyType)
			{
			case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
			}

			assert(false);
			return b2_staticBody;
		}

		inline Rigidbody2DComponent::BodyType Rigidbody2DTypeFromBox2DBody(b2BodyType bodyType)
		{
			switch (bodyType)
			{
			case b2_staticBody:    return Rigidbody2DComponent::BodyType::Static;
			case b2_dynamicBody:   return Rigidbody2DComponent::BodyType::Dynamic;
			case b2_kinematicBody: return Rigidbody2DComponent::BodyType::Kinematic;
			}

			assert(false);
			return Rigidbody2DComponent::BodyType::Static;
		}

	}

	class MyRayCastCallback : public b2RayCastCallback
	{
	public:
		MyRayCastCallback(UINT16 mask)
			:m_mask(mask)
		{
			m_fixture = NULL;
		}

		float ReportFixture(b2Fixture* fixture, const b2Vec2& point,
			const b2Vec2& normal, float fraction)
		{
			uint16 categoryBit = fixture->GetFilterData().categoryBits;

			if ((m_mask & categoryBit) == 0)
				return -1;
			else {
				m_fixture = fixture;
				m_point = point;
				m_normal = normal;
				m_fraction = fraction;
				return fraction;
			}
		}

		b2Fixture* m_fixture;
		b2Vec2 m_point;
		b2Vec2 m_normal;
		UINT16 m_mask;
		float m_fraction;
	};
}