#pragma once
#include "UUID.h"
namespace SY
{
	struct BoxCollider2DComponent
	{
		Vector2 Offset = Vector2::Zero;
		Vector2 Size = Vector2::One;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;
		UINT16 categoryBits = 1;
		UINT16 maskBits = 1;
		bool isSensor = false;

		void* RuntimeFixture;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
		static void DrawImGui(BoxCollider2DComponent& component);

		Vector2 parentCenter = Vector2::Zero;
		float parentAngle = 0.f;
	};

	struct CircleCollider2DComponent
	{
		Vector2 Offset = Vector2::Zero;
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;
		UINT16 categoryBits = 1;
		UINT16 maskBits = 1;
		bool isSensor = false;

		void* RuntimeFixture;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
		static void DrawImGui(CircleCollider2DComponent& component);

		Vector2 parentCenter = Vector2::Zero;
		float parentAngle = 0.f;
	};

	struct RevoluteJointComponent
	{
		UUID Opponent = 0;
		Vector2 LocalAnchor;
		Vector2 OpponentLocalAnchor;
		bool EnableLimit;
		Vector2 AngleRange;
		
		void* b2Joint;

		static void DrawImGui(RevoluteJointComponent& component);
	};

	struct DistanceJointComponent
	{
		UUID Opponent = 0;
		Vector2 LocalAnchor;
		Vector2 OpponentLocalAnchor;
		float minLength = 0.f;
		float maxLength = FLT_MAX;

		float stiffness = 0.f;
		float damping = 0.f;

		void* b2Joint;

		float GetCurrentLength();
		void SetMinLength(float minLength);
		float GetMinLength();
		static void DrawImGui(DistanceJointComponent& component);
	};
}
