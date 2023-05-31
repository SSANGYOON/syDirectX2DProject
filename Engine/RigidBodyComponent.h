#pragma once

namespace SY
{
	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		
		bool FixedRotation = false;
		bool DisableGravity = false;

		float LinearDamping = 0;
		float AngularDamping = 0;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;

		static void DrawImGui(Rigidbody2DComponent& component);
	};
}
