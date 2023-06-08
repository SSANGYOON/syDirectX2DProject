#pragma once

#include "SceneCamera.h"
#include "IDComponent.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "Collider.h"
#include "Animator.h"
#include "Drawable.h"
#include "Light.h"
#include "ParticleSystem.h"

#include "ConstantBuffer.h"
#include <DirectXMath.h>
#include "SimpleMath.h"

using namespace DirectX;
namespace SY {

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};
	enum class EntityState : UINT8
	{
		Active,
		Pause,
		Dead
	};

	struct Dead { int a = 0; };
	struct Pause { int a = 0; };
	struct Active { int a = 0; };

	struct StateComponent
	{
		EntityState state = EntityState::Active;
		EntityState currentState = EntityState::Active;
	};
	struct Parent
	{
		UUID parentHandle;
	};

	struct ScriptComponent
	{
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	struct BackGroundColorComponent
	{
		Vector4 color;
	};

	struct DontDestroy
	{
		Vector3 desiredPos;
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<StateComponent, TransformComponent, Parent, Light,
		CameraComponent, ScriptComponent, SpriteAnimatorComponent, TransformAnimatorComponent, BackGroundColorComponent, Bloom,
		Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent, RevoluteJointComponent, DistanceJointComponent, SpriteRendererComponent, CircleRendererComponent, TrailRenderer, LineRenderer,
		DontDestroy,
		RectTransformComponent, PanelComponent, SliderComponent, SlotComponent, IconComponent, Eraser, ParticleSystem>;
}

