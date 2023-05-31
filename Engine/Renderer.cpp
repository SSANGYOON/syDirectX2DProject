#include "pch.h"
#include "Renderer.h"
#include "EditorCamera.h"
#include "Resources.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "Resources.h"
#include "Engine.h"
#include "InstancingBuffer.h"
#include "ComputeShader.h"

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
		trCB.view = cameraTrans.Invert();
		trCB.projection = camera.GetProjection();

	}

	void Renderer::Begin(const EditorCamera& camera)
	{
		if (rect == nullptr)
			rect = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
		if (point == nullptr)
			point = GET_SINGLE(Resources)->Find<Mesh>(L"PointMesh");
		trCB.view = camera.GetView();
		trCB.projection = camera.GetProjection();
	}

	void Renderer::DrawDeffered()
	{
		auto shader = GET_SINGLE(Resources)->Find<Shader>(L"Lighting");
		shader->BindShader();

		auto tex = GEngine->GetRenderTargetTex(RENDER_TARGET_GROUP_TYPE::DEFFERED, 0);
		tex->BindSRV(ShaderStage::PS, 0);

		auto position = GEngine->GetRenderTargetTex(RENDER_TARGET_GROUP_TYPE::DEFFERED, 2);
		position->BindSRV(ShaderStage::PS, 1);

		auto lightMap = GEngine->GetRenderTargetTex(RENDER_TARGET_GROUP_TYPE::DEFFERED, 3);
		lightMap->BindSRV(ShaderStage::PS, 2);

		rect->BindBuffer();
		rect->Render();

		tex->ClearSRV(ShaderStage::PS, 0);
		position->ClearSRV(ShaderStage::PS, 1);
		lightMap->ClearSRV(ShaderStage::PS, 2);
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

		shared_ptr<ConstantBuffer> mc = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL);

		MaterialCB mcb = {};

		mcb.SetVec4(0, color);
		mc->SetData(&mcb);
		mc->SetPipline(ShaderStage::PS);

		rect->BindBuffer();
		rect->Render();
	}

	void Renderer::DrawCircle(const Matrix& transform, const Vector4& color, const entt::entity& entity)
	{
		stats.DrawCalls++;
		auto shader = GET_SINGLE(Resources)->Find<Shader>(L"CircleShader");
		auto circle = GET_SINGLE(Resources)->Find<Mesh>(L"Circle2D");
		shader->BindShader();

		trCB.world = transform;
		trCB.entity = (int)entity;

		shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::TRANSFORM);
		cb->SetData(&trCB);
		cb->SetPipline(ShaderStage::VS);

		shared_ptr<ConstantBuffer> mc = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL);

		MaterialCB mcb = {};

		mcb.SetVec4(0, color);
		mc->SetData(&mcb);
		mc->SetPipline(ShaderStage::PS);

		circle->BindBuffer();
		circle->Render();
	}

	void Renderer::DrawCircleOutline(const Matrix& transform, const Vector4& color, const entt::entity& entity)
	{
		stats.DrawCalls++;
		auto shader = GET_SINGLE(Resources)->Find<Shader>(L"DebugShader");
		auto circle = GET_SINGLE(Resources)->Find<Mesh>(L"CircleMesh");
		shader->BindShader();

		trCB.world = transform;
		trCB.entity = (int)entity;

		shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::TRANSFORM);
		cb->SetData(&trCB);
		cb->SetPipline(ShaderStage::VS);

		shared_ptr<ConstantBuffer> mc = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL);

		MaterialCB mcb = {};

		mcb.SetVec4(0, color);
		mc->SetData(&mcb);
		mc->SetPipline(ShaderStage::PS);

		circle->BindBuffer();
		circle->Render();
	}

	void Renderer::DrawRect(const Matrix& transform, shared_ptr<Material>& material, const entt::entity& entity, int instances)
	{
		DrawMesh(transform, material, rect, entity, instances);
	}

	void Renderer::DrawBatchSprite(shared_ptr<InstancingBuffer>& buffer, shared_ptr<Material>& material)
	{
		stats.DrawCalls++;
		buffer->PushData();
		stats.Instanced += buffer->GetCount() - 1;

		material->Bind();

		rect->Render(buffer);

		material->Clear();
	}

	void Renderer::DrawPoint(const Matrix& transform, shared_ptr<Material>& material, const entt::entity& entity)
	{
		DrawMesh(transform, material, point, entity);
	}

	void Renderer::DrawMesh(const Matrix& transform, shared_ptr<Material>& material, shared_ptr<Mesh>& mesh, const entt::entity& entity, int instances)
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
		mesh->RenderInstanced(instances);

		material->Clear();
	}

	void Renderer::ApplyBloom(float threshold, float intensity)
	{
		CONTEXT->OMSetRenderTargets(0, nullptr, nullptr);

		auto resizedEmission = GET_SINGLE(Resources)->Find<Texture>(L"DownScaledEmission");
		auto resizer = GET_SINGLE(Resources)->Find<Shader>(L"Resizer");

		CONTEXT->OMSetRenderTargets(1, resizedEmission->GetRTVRef(), nullptr);
		D3D11_VIEWPORT _viewPort = { 0.0f, 0.0f, 480.f, 270.f, 0.0f, 1.0f };
		CONTEXT->RSSetViewports(1, &_viewPort);

		resizer->BindShader();

		auto hdrbuffer = GEngine->GetRenderTargetTex(RENDER_TARGET_GROUP_TYPE::HDR, 0);
		auto emission = GEngine->GetRenderTargetTex(RENDER_TARGET_GROUP_TYPE::HDR, 1);

		emission->BindSRV(ShaderStage::PS, 0);
		resizer->BindShader();

		rect->BindBuffer();
		rect->Render();

		CONTEXT->OMSetRenderTargets(0, nullptr, nullptr);
		
		

		BloomCB blCB;

		blCB.texSize = { 480.f,270.f };
		blCB.bloomIntensity = intensity;
		blCB.bloomThreshold = threshold;

		shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::BLOOM);
		cb->SetData(&blCB);
		cb->SetPipline(ShaderStage::CS);
		cb->SetPipline(ShaderStage::PS);

		auto horizonalBlur = GET_SINGLE(Resources)->Find<Texture>(L"HorizontalBlur");
		auto horizonalBlurShader = GET_SINGLE(Resources)->Find<ComputeShader>(L"HorizonalBlurShader");

		resizedEmission->BindSRV(ShaderStage::CS,0);
		horizonalBlur->BindUAV(0);
		horizonalBlurShader->SetShaderGroup(2, 270, 1);

		horizonalBlurShader->Dispatch();
		resizedEmission->ClearSRV(ShaderStage::CS, 0);
		horizonalBlur->ClearUAV(0);

		auto verticalBlur = GET_SINGLE(Resources)->Find<Texture>(L"VerticalBlur");
		auto verticalBlurShader = GET_SINGLE(Resources)->Find<ComputeShader>(L"VerticalBlurShader");

		horizonalBlur->BindSRV(ShaderStage::CS, 0);
		verticalBlur->BindUAV(0);
		verticalBlurShader->SetShaderGroup(480, 2, 1);

		verticalBlurShader->Dispatch();
		horizonalBlur->ClearSRV(ShaderStage::CS, 0);
		verticalBlur->ClearUAV(0);

		GEngine->BindRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::EDITOR);

		hdrbuffer->BindSRV(ShaderStage::PS, 0);
		verticalBlur->BindSRV(ShaderStage::PS, 1);

		auto BloomCombline = GET_SINGLE(Resources)->Find<Shader>(L"BloomCombline");
		BloomCombline->BindShader();

		rect->BindBuffer();
		rect->Render();

		hdrbuffer->ClearSRV(ShaderStage::PS, 0);
		verticalBlur->ClearSRV(ShaderStage::PS, 1);
	}

	void Renderer::ACESMap()
	{
		GEngine->BindRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::EDITOR);
		auto hdrbuffer = GEngine->GetRenderTargetTex(RENDER_TARGET_GROUP_TYPE::HDR, 0);
		hdrbuffer->BindSRV(ShaderStage::PS, 0);

		auto BloomCombline = GET_SINGLE(Resources)->Find<Shader>(L"ACESMap");
		BloomCombline->BindShader();
		
		rect->BindBuffer();
		rect->Render();

		hdrbuffer->ClearSRV(ShaderStage::PS, 0);
	}

	void Renderer::End()
	{

	}
}
