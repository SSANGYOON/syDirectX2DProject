#pragma once

#include "SceneCamera.h"
#include "UUID.h"
#include "ConstantBuffer.h"
#include <DirectXMath.h>
#include "SimpleMath.h"
#include "Resources.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"

using namespace DirectX;
class Animation;
class TransformAnimation;
class Texture;
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
		EntityState currentState = EntityState::Active;
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

	struct SpriteRendererComponent
	{
		shared_ptr<Texture> Diffuse = nullptr;
		shared_ptr<Texture> LightMap = nullptr;

		shared_ptr<Shader> shader = nullptr;

		Vector2 sourceOffset;
		Vector2 sourceSize;
		Vector2 targetOffset;

		shared_ptr<Material> material;
		Vector4 Color = Vector4(1.f, 0, 1.f, 1);

		UINT16 LayerBit = 1;
		bool fixed = false;
		SpriteRendererComponent() : material(make_shared<Material>()) {}

		void SetMaterial()
		{
			if (shader)
				material->SetShader(shader);
			else
				material->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"SpriteShader"));

			material->SetTexture(0, Diffuse);
			material->SetTexture(1, LightMap);
			material->SetVec2(0, sourceOffset);
			material->SetVec2(1, sourceSize);
			if(Diffuse)
				material->SetVec2(2, Diffuse->GetSize());
			material->SetVec2(3, targetOffset);
			material->SetVec4(0, Color);
			material->SetUInt(0, fixed);
		}
	};

	struct PanelComponent
	{
		Vector2 offset;
		Vector2 tintOffset;
		Vector2 resizeTexture = Vector2::One;
		Vector4 color;
		shared_ptr<Texture> panel;
		shared_ptr<Material> material;

		PanelComponent() : material(make_shared<Material>()) {}

		void SetMaterial()
		{
			material->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"UIShader"));
			material->SetTexture(0, panel);
			if(panel)
				material->SetVec2(0, panel->GetSize() * resizeTexture);
			material->SetVec2(1, offset * resizeTexture);
			material->SetVec2(2, tintOffset * resizeTexture);
			material->SetVec4(0, color);
		}
	};

	struct SliderComponent
	{
		bool showNumeric;
		float currentValue;
		float maxValue;
		shared_ptr<Texture> bar;
		shared_ptr<Texture> gauge;
		shared_ptr<Material> material;

		SliderComponent() : material(make_shared<Material>()) {}

		void SetMaterial()
		{
			material->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"GaugeShader"));
			material->SetTexture(0, bar);
			material->SetTexture(1, gauge);
			material->SetVec2(0, bar->GetSize());
			material->SetVec2(1, gauge->GetSize());
			material->SetFloat(0, currentValue);
			material->SetFloat(1, maxValue);
			material->SetUInt(0, showNumeric);
		}
	};

	struct SlotComponent
	{
		float rotation = 0.f;
		Vector2 itemSize;
		shared_ptr<Texture> slot;
		shared_ptr<Texture> item;
		shared_ptr<Texture> slotMask;
		shared_ptr<Material> material;

		SlotComponent() : material(make_shared<Material>()) {}

		void SetMaterial() {
			material->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"SlotShader"));
			material->SetTexture(0, slot);
			material->SetTexture(1, item);
			material->SetTexture(2, slotMask);
			if(slot)
				material->SetVec2(0, slot->GetSize());
			if(item)
				material->SetVec2(1, item->GetSize());
			material->SetVec2(2, itemSize);
			material->SetFloat(0, rotation);
		}
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<StateComponent, TransformComponent, Parent,
		CameraComponent, ScriptComponent, SpriteAnimatorComponent, TransformAnimatorComponent, 
		Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent,
		SpriteRendererComponent, PanelComponent, SliderComponent, SlotComponent>;
}

