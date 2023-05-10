#include "pch.h"
#include "Drawable.h"

namespace SY {
	void SpriteRendererComponent::DrawImGui(SpriteRendererComponent& component)
	{
		ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&component.Color));

		ImGui::Text("Diffuse", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.Diffuse = texture;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.Diffuse) {
			ImGui::Text(wtos(component.Diffuse->GetPath()).c_str());

			if (ImGui::Button("Remove"))
			{
				component.Diffuse = nullptr;
			}
		}

		ImGui::InputFloat2("SourceOffset", reinterpret_cast<float*>(&component.sourceOffset));
		ImGui::InputFloat2("SourceSize", reinterpret_cast<float*>(&component.sourceSize));
		ImGui::Checkbox("Fixed", &component.fixed);

		bool layerBits[8];
		int category = component.LayerBit;
		for (int i = 0; i < 8; i++)
		{
			layerBits[i] = category % 2;
			category /= 2;
		}

		ImGui::Text("Layer");
		for (int i = 0; i < 8; i++) {
			ImGui::Checkbox((to_string(i)).c_str(), &layerBits[i]);
			if (i < 7)
				ImGui::SameLine();
		}

		component.LayerBit = 0;
		for (int i = 0; i < 8; i++)
		{
			component.LayerBit += layerBits[i] * (1 << i);
		}
	}

	void PanelComponent::DrawImGui(PanelComponent& component)
	{
		ImGui::DragFloat2("Offset", reinterpret_cast<float*>(&component.offset));
		ImGui::DragFloat2("TintRange", reinterpret_cast<float*>(&component.tintOffset));
		ImGui::DragFloat2("ResizeTexture", reinterpret_cast<float*>(&component.resizeTexture));
		ImGui::ColorEdit4("TintColor", reinterpret_cast<float*>(&component.color));
		ImGui::Text("Panel", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.panel = texture;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.panel) {
			ImGui::Text(wtos(component.panel->GetPath()).c_str());

			if (ImGui::Button("Remove"))
			{
				component.panel = nullptr;
			}
		}
	}
	void SliderComponent::DrawImGui(SliderComponent& component)
	{
		ImGui::DragFloat("currentValue", &component.currentValue);
		ImGui::DragFloat("maxValue", &component.maxValue);

		ImGui::Text("BarTexture", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.bar = texture;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.bar) {
			ImGui::Text(wtos(component.bar->GetPath()).c_str());

			if (ImGui::Button("Remove bar"))
			{
				component.bar = nullptr;
			}
		}

		ImGui::Text("GaugeTexture", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.gauge = texture;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.gauge) {
			ImGui::Text(wtos(component.gauge->GetPath()).c_str());

			if (ImGui::Button("Remove gauge"))
			{
				component.gauge = nullptr;
			}
		}
	}
	void SlotComponent::DrawImGui(SlotComponent& component)
	{
		float rotation = component.rotation * 180.f / XM_PI;
		ImGui::DragFloat("Rotation", &rotation);
		component.rotation = rotation / 180.f * XM_PI;

		ImGui::DragFloat2("ItemSize", reinterpret_cast<float*>(&component.itemSize));

		ImGui::Text("Slot", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.slot = texture;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.slot) {
			ImGui::Text(wtos(component.slot->GetPath()).c_str());

			if (ImGui::Button("Remove slot"))
				component.slot = nullptr;
		}

		ImGui::Text("SlotMask", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.slotMask = texture;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.slotMask) {
			ImGui::Text(wtos(component.slotMask->GetPath()).c_str());

			if (ImGui::Button("Remove slotmask"))
				component.slotMask = nullptr;
		}

		ImGui::Text("Item", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.item = texture;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.item) {
			ImGui::Text(wtos(component.item->GetPath()).c_str());

			if (ImGui::Button("Remove item"))
				component.item = nullptr;
		}
	}
	void IconComponent::DrawImGui(IconComponent& component)
	{
		ImGui::Text("Icon", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);


				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.icon = texture;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.icon) {
			ImGui::Text(wtos(component.icon->GetPath()).c_str());

			if (ImGui::Button("Remove item"))
				component.icon = nullptr;
		}

		ImGui::ColorEdit4("Tint color", reinterpret_cast<float*>(&component.tint));
	}
}
