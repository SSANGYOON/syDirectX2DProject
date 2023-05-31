#pragma once
#include "entt.hpp"
#include "TransformComponent.h"
#include "StructuredBuffer.h"

class ComputeShader;
class Texture;
class Material;
class Shader;
namespace SY
{

#pragma pack(1)
	struct Particle
	{
		Vector3 position;
		float remainLife;
		Vector2 velocity;
		Vector2 size;
		UINT active;
		Vector3 padding;
	};

	struct ParticleShared
	{
		UINT activeCount;
		Vector3 padding;
	};
#pragma pack()

	enum class ParticleState
	{
		NORMAL,
		UpdateOnly,
		Pause,
	};

	struct ParticleSystem
	{
		ParticleSystem();
		void Init();

		void OnUpdate(float timeStep, TransformComponent& tr);
		void OnRender(TransformComponent& tr, entt::entity entity);

		shared_ptr<ComputeShader> mCustomShader;
		shared_ptr<Material> mMaterial;

		shared_ptr<StructuredBuffer> mBuffer;
		shared_ptr<StructuredBuffer> mSharedBuffer;

		Vector2 Position;
		Vector2 PositionVariation;

		Vector2 AttachOffset;

		Vector2 Velocity, VelocityVariation, VelocityEnd;
		Vector4 ColorBegin, ColorEnd;
		Vector4 EmissionBegin, EmissionEnd;

		int MaxParticles;
		float Frequency;

		Vector2 SizeBegin, SizeEnd, SizeVariation;

		float LifeTime = 0.f;
		float accTime = 0.f;

		bool PositionPolar = false;
		bool VelocityPolar = false;
		bool TextureAttach = false;

		shared_ptr<Texture> attachTexture;
		shared_ptr<Texture> graphicsTexture;

		ParticleState state = ParticleState::NORMAL;

		static void DrawImGui(ParticleSystem& component);
	};
}

