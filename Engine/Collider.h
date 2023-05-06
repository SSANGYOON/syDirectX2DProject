#pragma once

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

		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
		static void DrawImGui(BoxCollider2DComponent& component);
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

		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
		static void DrawImGui(CircleCollider2DComponent& component);
	};
}
