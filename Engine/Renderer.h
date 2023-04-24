#pragma once
#include "entt.hpp"

class Texture;
class Shader;
class Entity;
struct SpriteCB;
struct TransformCB;

namespace SY {
	class EditorCamera;
	class Camera;

	struct RenderrerStat
	{
		UINT DrawCalls;
		UINT Instanced;
	};
	class Renderer
	{
	public:
		static void Begin(const Camera& camera, const Matrix& cameraTrans);
		static void Begin(const EditorCamera& camera);
		static void DrawRectOutline(const Matrix& transform, const Vector4& color, const entt::entity & entity);
		static void DrawCircleOutline(const Matrix& transform, const Vector4& color, const entt::entity& entity);
		static void DrawSprite(const Matrix& transform, SpriteCB& spCB, const entt::entity& entity, shared_ptr<Texture> diffuse = nullptr, shared_ptr<Texture> light = nullptr, shared_ptr<Shader> customShader = nullptr);

		static const RenderrerStat& GetStats() { return stats; }
		static void ResetStats() { stats = {}; }
		static void End();

		static TransformCB trCB;
		static RenderrerStat stats;
	};

}