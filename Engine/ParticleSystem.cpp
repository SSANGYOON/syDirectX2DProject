#include "pch.h"
#include "ParticleSystem.h"
#include "ConstantBuffer.h"
#include "ComputeShader.h"
#include "Engine.h"
#include "Resources.h"
#include "Material.h"
#include "Renderer.h"
#include "Entity.h"
#include "Shader.h"

namespace SY {

	ParticleSystem::ParticleSystem()
		: MaxParticles(256)
		, Frequency(1.f)
		, Position{}, PositionVariation{}
		, Velocity{}, VelocityVariation{}, VelocityEnd{}
		, ColorBegin{}, ColorEnd{}
		, SizeBegin(0.f), SizeEnd(0.f), SizeVariation(0.f)
		, mCustomShader(nullptr)
		, LifeTime(0.f)
	{
		mMaterial = make_shared<Material>();

		mMaterial->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"ParticleGraphics"));
	}
	void ParticleSystem::Init()
	{
		mBuffer = make_shared<StructuredBuffer>();

		Particle* a = new Particle[1024];

		memset(a, 0, sizeof(Particle) * 1024);
		mBuffer->Create(sizeof(Particle), 1024, a, true, true);

		delete[] a;

		mSharedBuffer = make_shared<StructuredBuffer>();
		mSharedBuffer->Create(sizeof(ParticleShared), 1, nullptr, true, true);
	}
	void ParticleSystem::OnUpdate(float timeStep, TransformComponent& tr)
	{
		if (state == ParticleState::Pause)
			return;

		float genTime = 1.0f / Frequency;
		//누적시간
		accTime += timeStep;

		UINT addCount = 0;
		if (state == ParticleState::NORMAL) {
			if (genTime < accTime)
			{
				float f = (accTime / genTime);
				UINT iAliveCount = (UINT)f;
				accTime -= iAliveCount * genTime;

				addCount += iAliveCount;
			}
		}
		else if (accTime >= LifeTime)
			state = ParticleState::Pause;
		
		ParticleCB pcb = {};

		pcb.Position = Position;
		pcb.PositionVariation = PositionVariation;

		pcb.SizeBegin = SizeBegin;
		pcb.SizeEnd = SizeEnd;
		pcb.SizeVariation = SizeVariation;

		pcb.Velocity = Velocity;
		pcb.VelocityVariation = VelocityVariation;
		pcb.VelocityEnd = VelocityEnd;

		pcb.ColorBegin = ColorBegin;
		pcb.ColorEnd = ColorEnd;

		pcb.EmissionBegin = EmissionBegin;
		pcb.EmissionEnd = EmissionEnd;

		pcb.worldTrans = tr.localToWorld;

		pcb.LifeTime = LifeTime;
		pcb.addCount = addCount;
		pcb.PositionPolar = PositionPolar;
		pcb.VelocityPolar = VelocityPolar;
		pcb.maxParticles = MaxParticles;
		pcb.textureAttach = TextureAttach;
		pcb.texturePos = AttachOffset;

		pcb.aliveZone = aliveZone;
		pcb.useAliveZone = UseAliveZone;
		pcb.useLocalCoord = UseLocalCoord;

		auto cb = GEngine->GetConstantBuffer(Constantbuffer_Type::PARTICLE);
		cb->SetData(&pcb);


		cb->SetPipline(ShaderStage::CS);
		cb->SetPipline(ShaderStage::PS);
		cb->SetPipline(ShaderStage::GS);

		mBuffer->BindUAV(0);
		mSharedBuffer->BindUAV(1);

		if (attachTexture)
			attachTexture->BindSRV(ShaderStage::CS, 0);

		shared_ptr<ComputeShader> cs;
		if (mCustomShader == nullptr)
			GET_SINGLE(Resources)->Find<ComputeShader>(L"ParticleShader")->Dispatch();
		else
			mCustomShader->Dispatch();

		mBuffer->Clear();
		mSharedBuffer->Clear();
	}
	void ParticleSystem::OnRender(TransformComponent& tr, entt::entity entity)
	{
		if (state == ParticleState::Pause)
			return;

		mBuffer->BindSRV(4);

		mMaterial->SetTexture(0, graphicsTexture);

		Renderer::DrawRect(tr.localToWorld, mMaterial, entity, mBuffer->GetStride());
		mBuffer->Clear();
	}
	void ParticleSystem::DrawImGui(ParticleSystem& component)
	{
		const char* stateString[] = { "Normal", "Update Only", "Pause" };
		const char* currentStateString = stateString[(int)component.state];

		if (ImGui::BeginCombo("State", currentStateString))
		{
			for (int i = 0; i < 3; i++)
			{
				const char* stateKey = stateString[i];
				bool isSelected = strcmp(stateKey, currentStateString) == 0;
				if (ImGui::Selectable(stateKey, isSelected))
					component.state = (ParticleState)i;

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		auto shaders = GET_SINGLE(Resources)->GetShaders();
		string skey;

		skey = wtos(component.mMaterial->GetShader()->GetKey());
		const char* currentShader = skey.c_str();

		if (ImGui::BeginCombo("Shader", currentShader))
		{
			for (int i = 0; i < shaders.size(); i++)
			{
				string s = wtos(shaders[i]->GetKey());
				const char* currentShaderKey = s.c_str();
				bool isSelected = strcmp(currentShader, currentShaderKey) == 0;
				if (ImGui::Selectable(currentShaderKey, isSelected))
					component.mMaterial->SetShader(shaders[i]);

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Text("Attach Texture", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.attachTexture = texture;

				component.TextureAttach = 1;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.attachTexture) {
			ImGui::Text(wtos(component.attachTexture->GetPath()).c_str());

			ImGui::DragFloat2("Attach Offset", reinterpret_cast<float*>(&component.AttachOffset));

			if (ImGui::Button("Remove attach"))
			{
				component.attachTexture = nullptr;
				component.TextureAttach = 0;
			}
		}

		ImGui::Text("Graphics Texture", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.graphicsTexture = texture;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.graphicsTexture) {
			ImGui::Text(wtos(component.graphicsTexture->GetPath()).c_str());

			if (ImGui::Button("Remove graphics"))
				component.graphicsTexture = nullptr;
		}

		ImGui::DragFloat2("Position", reinterpret_cast<float*>(&component.Position));
		ImGui::DragFloat2("PositionVariation", reinterpret_cast<float*>(&component.PositionVariation));

		ImGui::Checkbox("PositionPolar", &component.PositionPolar);

		ImGui::DragFloat2("Velocity", reinterpret_cast<float*>(&component.Velocity));
		ImGui::DragFloat2("VelocityEnd", reinterpret_cast<float*>(&component.VelocityEnd));
		ImGui::DragFloat2("VelocityVariation", reinterpret_cast<float*>(&component.VelocityVariation));

		ImGui::Checkbox("VelocityPolar", &component.VelocityPolar);

		ImGui::DragFloat2("SizeBegin", reinterpret_cast<float*>(&component.SizeBegin));
		ImGui::DragFloat2("SizeEnd", reinterpret_cast<float*>(&component.SizeEnd));
		ImGui::DragFloat2("SizeVariation", reinterpret_cast<float*>(&component.SizeVariation));

		ImGui::ColorEdit4("ColorBegin", reinterpret_cast<float*>(&component.ColorBegin));
		ImGui::ColorEdit4("ColorEnd", reinterpret_cast<float*>(&component.ColorEnd));

		ImGui::ColorEdit4("EmissionBegin", reinterpret_cast<float*>(&component.EmissionBegin), ImGuiColorEditFlags_::ImGuiColorEditFlags_HDR);
		ImGui::ColorEdit4("EmissionEnd", reinterpret_cast<float*>(&component.EmissionEnd), ImGuiColorEditFlags_::ImGuiColorEditFlags_HDR);

		ImGui::DragInt("MaxParticles", &component.MaxParticles);
		ImGui::DragFloat("Frequency", &component.Frequency);

		ImGui::DragFloat("LifeTime", &component.LifeTime);

		ImGui::Checkbox("UseLocalCoord", &component.UseLocalCoord);
		ImGui::Checkbox("UseAliveZone", &component.UseAliveZone);
		
		if(component.UseAliveZone)
			ImGui::DragFloat2("AliceZone", reinterpret_cast<float*>(&component.aliveZone));
	}
}

