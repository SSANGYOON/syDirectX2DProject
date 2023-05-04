#include "pch.h"
#include "Renderer.h"
#include "EditorCamera.h"
#include "Resources.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "Resources.h"
#include "Engine.h"

namespace SY {
	TransformCB Renderer::trCB = {};
	RenderrerStat Renderer::stats = {};

	shared_ptr<Mesh> Renderer::rect = nullptr;
	shared_ptr<Mesh> Renderer::point = nullptr;

	void Renderer::Begin(const Camera& camera, const Matrix& cameraTrans)
	{
		if (rect == nullptr)
			rect = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
		if (point == nullptr)
			point = GET_SINGLE(Resources)->Find<Mesh>(L"PointMesh");
		stats = {};
		trCB.view = cameraTrans.Invert();
		trCB.projection = camera.GetProjection();

	}

	void Renderer::Begin(const EditorCamera& camera)
	{
		if (rect == nullptr)
			rect = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
		if (point == nullptr)
			point = GET_SINGLE(Resources)->Find<Mesh>(L"PointMesh");
		stats = {};
		trCB.view = camera.GetView();
		trCB.projection = camera.GetProjection();
	}

	void Renderer::DrawRectOutline(const Matrix& transform, const Vector4& color, const entt::entity& entity)
	{
		auto shader = GET_SINGLE(Resources)->Find<Shader>(L"DebugShader");
		auto rect = GET_SINGLE(Resources)->Find<Mesh>(L"OutlinedRect");
		shader->BindShader();
		stats.DrawCalls++;
		trCB.world = transform;
		trCB.entity = (int)entity;

		shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::TRANSFORM);
		cb->SetData(&trCB);
		cb->SetPipline(ShaderStage::VS);
		cb->SetPipline(ShaderStage::PS);
		cb->SetPipline(ShaderStage::GS);

		rect->BindBuffer();
		rect->Render();
	}

	void Renderer::DrawCircleOutline(const Matrix& transform, const Vector4& color, const entt::entity& entity)
	{
		stats.DrawCalls++;
		auto shader = GET_SINGLE(Resources)->Find<Shader>(L"DebugShader");
		auto circle = GET_SINGLE(Resources)->Find<Mesh>(L"Circle2D");
		shader->BindShader();

		trCB.world = transform;
		trCB.entity = (int)entity;

		shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::TRANSFORM);
		cb->SetData(&trCB);
		cb->SetPipline(ShaderStage::VS);
		cb->SetPipline(ShaderStage::PS);
		cb->SetPipline(ShaderStage::GS);

		circle->BindBuffer();
		circle->Render();
	}

	void Renderer::DrawRect(const Matrix& transform, shared_ptr<Material>& material, const entt::entity& entity)
	{
		DrawMesh(transform, material, rect, entity);
	}

	void Renderer::DrawPoint(const Matrix& transform, shared_ptr<Material>& material, const entt::entity& entity)
	{
		DrawMesh(transform, material, point, entity);
	}

	void Renderer::DrawMesh(const Matrix& transform, shared_ptr<Material>& material, shared_ptr<Mesh>& mesh, const entt::entity& entity)
	{
		stats.DrawCalls++;
		

		trCB.world = transform;
		trCB.entity = (int)entity;

		shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::TRANSFORM);
		cb->SetData(&trCB);
		cb->SetPipline(ShaderStage::VS);
		cb->SetPipline(ShaderStage::PS);
		cb->SetPipline(ShaderStage::GS);

		material->Bind();

		mesh->BindBuffer();
		mesh->Render();

		material->Clear();
	}

	void Renderer::End()
	{
		// TODO for Instancing;
	}
}
