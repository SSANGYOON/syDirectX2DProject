#pragma once
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Resources.h"
struct Vertex;
namespace SY
{
	struct SpriteRendererComponent
	{
		shared_ptr<Texture> Diffuse = nullptr;
		shared_ptr<Texture> LightMap = nullptr;

		shared_ptr<Shader> shader = nullptr;

		Vector2 tile;
		Vector2 Offset;

		shared_ptr<Material> material;

		Vector4 Color = Vector4(1.f, 1, 1.f, 1);
		Vector4 Emission = Vector4::Zero;

		bool Deffered = false;
		UINT16 LayerBit = 1;
		SpriteRendererComponent() : material(make_shared<Material>()) {}

		void SetMaterial()
		{
			if (shader)
				material->SetShader(shader);

			else if(Deffered)
				material->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"SpriteDeffered"));
			else
				material->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"SpriteFoward"));

			material->SetTexture(0, Diffuse);
			material->SetTexture(1, LightMap);
		}

		static void DrawImGui(SpriteRendererComponent& component);
	};

	struct CircleRendererComponent
	{
		Vector4 Color = Vector4(1.f, 1, 1.f, 1);
		static void DrawImGui(CircleRendererComponent& component);
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
			if (panel)
				material->SetVec2(0, panel->GetSize() * resizeTexture);
			material->SetVec2(1, offset * resizeTexture);
			material->SetVec2(2, tintOffset * resizeTexture);
			material->SetVec4(0, color);
		}

		static void DrawImGui(PanelComponent& component);
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
			material->SetInt(0, showNumeric);
		}

		static void DrawImGui(SliderComponent& component);
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
			if (slot)
				material->SetVec2(0, slot->GetSize());
			if (item)
				material->SetVec2(1, item->GetSize());
			material->SetVec2(2, itemSize);
			material->SetFloat(0, rotation);
		}

		static void DrawImGui(SlotComponent& component);
	};

	struct IconComponent
	{
		shared_ptr<Texture> icon;
		Vector4 tint;
		shared_ptr<Material> material;

		IconComponent() : material(make_shared<Material>()) {}

		void SetMaterial() {
			material->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"IconShader"));
			material->SetTexture(0, icon);
			material->SetVec4(0, tint);
		}

		static void DrawImGui(IconComponent& component);
	};

	struct UIText
	{
		wstring text = L"";
		static void DrawImGui(UIText& component);
	};

	struct Eraser
	{
		shared_ptr<Material> material;

		Eraser() : material(make_shared<Material>()) {}

		void SetMaterial() {
			material->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"Eraser"));
		}

		static void DrawImGui(Eraser& component);
	};

	struct Bloom
	{
		float Threshold = 1.f;
		float Intensity = 0.f;

		Bloom() = default;
		Bloom(const Bloom&) = default;

		static void DrawImGui(Bloom& component);
	};
	
	struct TrailRenderer
	{
		Vector3 base;
		Vector3 tip;

		Vector4 Color;

		shared_ptr<Material> material;
		shared_ptr<Mesh> _mesh;

		

		float recoordFreq = 1.f;
		int currentIndex = -1;

		UINT maxRecoord = 300;
		float _accTime = 0.f;

		UINT recoorded = 1;
		vector<Vertex> _vertexes;
		vector<UINT> _indexes;
		TrailRenderer()
		{
			material = make_shared<Material>();
		}
		TrailRenderer(const TrailRenderer&) = default;

		void Init();
		void SetData();
		static void DrawImGui(TrailRenderer& component);
	};

	struct LineRenderer
	{
		Vector4 Color;
		Vector4 Emission;
		Vector2 Size;
		Vector2 Velocity;
		shared_ptr<Material> material;

		LineRenderer()
		{
			material = make_shared<Material>();
			material->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"Laser"));
		}
		LineRenderer(const LineRenderer&) = default;

		static void DrawImGui(LineRenderer& component);
	};
}

