#pragma once

#include "SceneCamera.h"
#include "Texture.h"
#include "UUID.h"
#include "ConstantBuffer.h"
#include <DirectXMath.h>
#include "SimpleMath.h"

using namespace DirectX;
class Animation;
class TransformAnimation;
namespace SY {

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(UUID id)
			: ID(id) {}
	};

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
	struct StateComponent
	{
		EntityState state = EntityState::Active;
	};

	struct TransformComponent
	{
		Vector3 translation;
		Vector3 scale;
		Vector3 rotation;
		Matrix localToParent;
		Matrix localToWorld;
	public:

		TransformComponent() : scale(Vector3::One), rotation(Vector3::Zero), translation(Vector3::Zero) {}
		TransformComponent(const TransformComponent&) = default;

		void CreateToParent()
		{
			Matrix matScale = Matrix::CreateScale(scale);
			Matrix matRotation = Matrix::CreateFromYawPitchRoll(rotation);
			Matrix matTranslation = Matrix::CreateTranslation(translation);
			localToParent = matScale * matRotation * matTranslation;
		}
	};

	struct Parent
	{
		UUID parentHandle;
	};

	struct SpriteRendererComponent
	{
		shared_ptr<Texture> Texture;
		SpriteCB spCB;
		Vector4 Color = Vector4(1.f, 0, 1.f, 1);
		UINT16 LayerBit = 1;
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to Scene
		bool FixedAspectRatio = false;

		UINT16 LayerBit = 65535;
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	class ScriptableEntity;
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	// Physics

	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		Vector2 Offset = Vector2::Zero;
		Vector2 Size = Vector2::One;

		// TODO(Yan): move into physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;
		UINT16 categoryBits = 1;
		UINT16 maskBits = 1;
		bool isSensor = false;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		Vector2 Offset = Vector2::Zero;
		float Radius = 0.5f;

		// TODO(Yan): move into physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;
		UINT16 categoryBits = 1;
		UINT16 maskBits = 1;
		bool isSensor = false;
		
		// Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	
	struct SpriteAnimatorComponent
	{
		float _currentTime = 0.f;
		float _lastTime = -1.f;
		float _endedAt = 0.f;
		shared_ptr<Animation> _currentClip;
		map<string, shared_ptr<Animation>> clips;
		map<string, string> _startEvent;
		map<string, string> _endEvent;
	};

	struct TransformAnimatorComponent
	{
		float _currentTime = 0.f;
		float _lastTime = -1.f;
		float _endedAt = 0.f;
		float _transitionTime = 0.f;
		float _transitionElapsed = 0.f;
		shared_ptr<TransformAnimation> _currentClip;
		map<string, shared_ptr<TransformAnimation>> clips;
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<StateComponent, TransformComponent, SpriteRendererComponent,
		CameraComponent, ScriptComponent, SpriteAnimatorComponent, TransformAnimatorComponent, Parent,
		NativeScriptComponent, Rigidbody2DComponent, BoxCollider2DComponent,
		CircleCollider2DComponent>;

}

