#pragma once
#include "Material.h"
#include "Texture.h"
#include "Shader.h";
#include "Resources.h"
namespace SY
{
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
			if (Diffuse)
				material->SetVec2(2, Diffuse->GetSize());
			material->SetVec2(3, targetOffset);
			material->SetVec4(0, Color);
			material->SetUInt(0, fixed);
		}

		static void DrawImGui(SpriteRendererComponent& component);
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
			material->SetUInt(0, showNumeric);
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
}

