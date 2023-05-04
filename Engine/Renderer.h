#pragma once
#include "entt.hpp"


class Entity;
class Mesh;
class Material;
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
		static void DrawRect(const Matrix& transform, shared_ptr<Material>& material, const entt::entity& entity);
		static void DrawPoint(const Matrix& transform, shared_ptr<Material>& material, const entt::entity& entity);
		static void DrawMesh(const Matrix& transform, shared_ptr<Material>& material, shared_ptr<Mesh>& mesh, const entt::entity& entity);

		static const RenderrerStat& GetStats() { return stats; }
		static void ResetStats() { stats = {}; }
		static void End();

		static shared_ptr<Mesh> rect;
		static shared_ptr<Mesh> point;
		static TransformCB trCB;
		static RenderrerStat stats;
	};

}