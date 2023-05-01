#include "pch.h"
#include "Renderer.h"
#include "EditorCamera.h"
#include "Resources.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "Resources.h"
#include "Engine.h"

namespace SY {
	TransformCB Renderer::trCB = {};
	RenderrerStat Renderer::stats = {};

	void Renderer::Begin(const Camera& camera, const Matrix& cameraTrans)
	{
		stats = {};
		trCB.view = cameraTrans.Invert();
		trCB.projection = camera.GetProjection();

	}

	void Renderer::Begin(const EditorCamera& camera)
	{
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

	void Renderer::DrawSprite(const Matrix& transform, SpriteCB& spCB, const entt::entity& entity, shared_ptr<Texture> diffuse, shared_ptr<Texture> lighted, shared_ptr<Shader> customShader)
	{
		stats.DrawCalls++;
		auto rect = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
		shared_ptr<Shader> shader;
		if (customShader)
			shader = customShader;
		else
			shader = GET_SINGLE(Resources)->Find<Shader>(L"SpriteShader");
		shader->BindShader();

		trCB.world = transform;
		trCB.entity = (int)entity;

		shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::TRANSFORM);
		cb->SetData(&trCB);
		cb->SetPipline(ShaderStage::VS);
		cb->SetPipline(ShaderStage::PS);
		cb->SetPipline(ShaderStage::GS);

		shared_ptr<ConstantBuffer> spcb = GEngine->GetConstantBuffer(Constantbuffer_Type::SPRITE);

		if (diffuse)
		{
			spCB.textured = 1;
			diffuse->BindSRV(ShaderStage::PS, 0);

		}
		else
		{
			spCB.textured = 0;
		}
		if (lighted)
		{
			spCB.lighted = 1;
			lighted->BindSRV(ShaderStage::PS, 1);
		}
		else
		{
			spCB.lighted = 0;
		}

		spcb->SetData(&spCB);
		spcb->SetPipline(ShaderStage::VS);
		spcb->SetPipline(ShaderStage::PS);

		rect->BindBuffer();
		rect->Render();

		if (diffuse)
			diffuse->ClearSRV(ShaderStage::PS, 0);

		if (lighted)
			lighted->ClearSRV(ShaderStage::PS, 1);
	}

	void Renderer::DrawMesh(const Matrix& transform, const entt::entity& entity, shared_ptr<Shader> shader, shared_ptr<Mesh> mesh, shared_ptr<Texture> diffuse, shared_ptr<Texture> additional1, shared_ptr<Texture> additional2)
	{
		stats.DrawCalls++;
		shader->BindShader();

		trCB.world = transform;
		trCB.entity = (int)entity;

		shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::TRANSFORM);
		cb->SetData(&trCB);
		cb->SetPipline(ShaderStage::VS);
		cb->SetPipline(ShaderStage::PS);
		cb->SetPipline(ShaderStage::GS);

		if(diffuse)
			diffuse->BindSRV(ShaderStage::PS, 0);
		if (additional1)
			additional1->BindSRV(ShaderStage::PS, 1);
		if (additional2)
			additional2->BindSRV(ShaderStage::PS, 2);

		mesh->BindBuffer();
		mesh->Render();

		if (diffuse)
			diffuse->ClearSRV(ShaderStage::PS, 0);
		if (additional1)
			additional1->ClearSRV(ShaderStage::PS, 1);
		if (additional2)
			additional2->ClearSRV(ShaderStage::PS, 2);
	}

	void Renderer::End()
	{
		// TODO for Instancing;
	}
}
