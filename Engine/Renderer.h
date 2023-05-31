#pragma once
#include "entt.hpp"


class Entity;
class Mesh;
class Material;
class InstancingBuffer;
class StructuredBuffer;
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
		static void DrawDeffered();
		static void DrawRectOutline(const Matrix& transform, const Vector4& color, const entt::entity & entity);
		static void DrawCircle(const Matrix& transform, const Vector4& color, const entt::entity& entity);
		static void DrawCircleOutline(const Matrix& transform, const Vector4& color, const entt::entity& entity);
		static void DrawRect(const Matrix& transform, shared_ptr<Material>& material, const entt::entity& entity, int instances = 1);
		static void DrawBatchSprite(shared_ptr<InstancingBuffer>& buffer, shared_ptr<Material>& material);
		static void DrawPoint(const Matrix& transform, shared_ptr<Material>& material, const entt::entity& entity);
		static void DrawMesh(const Matrix& transform, shared_ptr<Material>& material, shared_ptr<Mesh>& mesh, const entt::entity& entity, int instances = 1);
		static void ApplyBloom(float threshold, float intensity);
		static void ACESMap();

		static const RenderrerStat& GetStats() { return stats; }
		static void ResetStats() { stats = {}; }
		static void End();

		static shared_ptr<Mesh> rect;
		static shared_ptr<Mesh> point;
		static TransformCB trCB;
		static RenderrerStat stats;
	};

}