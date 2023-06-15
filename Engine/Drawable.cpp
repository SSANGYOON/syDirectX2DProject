#include "pch.h"
#include "Drawable.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"

namespace SY {

	void DrawMaterialParams(shared_ptr<Material> material)
	{
		if (ImGui::TreeNodeEx("Int params")) {
			for (int i = 0; i < 4; i++)
			{
				string label = "Int : " + to_string(i);
				int v = material->GetInt(i);
				ImGui::InputInt(label.c_str(), &v);
				material->SetInt(i, v);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Float params")) {
			for (int i = 0; i < 4; i++)
			{
				string label = "float : " + to_string(i);
				float v = material->GetFloat(i);
				ImGui::InputFloat(label.c_str(), &v);
				material->SetFloat(i, v);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Vector2 params")) {
			for (int i = 0; i < 4; i++)
			{
				string label = "Vec2 : " + to_string(i);
				auto v = material->GetVec2(i);
				ImGui::InputFloat2(label.c_str(), reinterpret_cast<float*>(&v));
				material->SetVec2(i, v);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Vector4 params")) {
			for (int i = 0; i < 4; i++)
			{
				string label = "Vec4 : " + to_string(i);
				auto v = material->GetVec4(i);
				ImGui::InputFloat4(label.c_str(), reinterpret_cast<float*>(&v));
				material->SetVec4(i, v);
			}
			ImGui::TreePop();
		}
	}

	void SpriteRendererComponent::DrawImGui(SpriteRendererComponent& component)
	{
		ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&component.Color));
		ImGui::ColorEdit4("Emission", reinterpret_cast<float*>(&component.Emission), ImGuiColorEditFlags_::ImGuiColorEditFlags_HDR);


		auto shaders = GET_SINGLE(Resources)->GetShaders();
		string skey;
		if (component.shader)
			skey = wtos(component.shader->GetKey());
		const char* currentShader = component.shader == nullptr?  "SpriteShader" : skey.c_str();
		if (ImGui::BeginCombo("Shader", currentShader))
		{
			for (int i = 0; i < shaders.size(); i++)
			{
				string s = wtos(shaders[i]->GetKey());
				const char* currentShaderKey = s.c_str();
				bool isSelected = strcmp(currentShader, currentShaderKey) == 0;
				if (ImGui::Selectable(currentShaderKey, isSelected))
				{
					component.shader = shaders[i];
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

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
				component.tile = texture->GetSize();
			}
			ImGui::EndDragDropTarget();
		}



		if (component.Diffuse) {
			ImGui::Text(wtos(component.Diffuse->GetPath()).c_str());

			if (ImGui::Button("Remove Diffuse"))
			{
				component.Diffuse = nullptr;
			}
		}

		ImGui::Text("LightMap", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.LightMap = texture;
			}
			ImGui::EndDragDropTarget();
		}

		if (component.LightMap) {
			ImGui::Text(wtos(component.LightMap->GetPath()).c_str());

			if (ImGui::Button("Remove LightMap"))
			{
				component.LightMap = nullptr;
			}
		}

		ImGui::InputFloat2("Tile", reinterpret_cast<float*>(&component.tile));
		ImGui::InputFloat2("Offset", reinterpret_cast<float*>(&component.Offset));

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

		ImGui::Checkbox("Deffered", &component.Deffered);

		DrawMaterialParams(component.material);
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
				component.panel = nullptr;
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
	void Eraser::DrawImGui(Eraser& component)
	{
	}
	void Bloom::DrawImGui(Bloom& component)
	{
		ImGui::DragFloat("Threshold", &component.Threshold);
		ImGui::DragFloat("Intensity", &component.Intensity);
	}
	void CircleRendererComponent::DrawImGui(CircleRendererComponent& component)
	{
		ImGui::ColorEdit4("Circle Color", reinterpret_cast<float*>(&component.Color));
	}
	void TrailRenderer::Init()
	{
		_mesh = make_shared<Mesh>();
		_vertexes.resize(maxRecoord * 2);
		recoorded = 1;
		_mesh->CreateVertexBuffer(_vertexes.data(), maxRecoord * 2, D3D11_USAGE::D3D11_USAGE_DYNAMIC);

		_indexes.resize(maxRecoord * 2);

		for (int i = 0; i < maxRecoord*2; i++)
		{
			_indexes[i] = i;
		}

		_mesh->CreateIndexBuffer(_indexes.data(), maxRecoord * 2, D3D11_USAGE::D3D11_USAGE_DYNAMIC);
	}
	void TrailRenderer::SetData()
	{
		_mesh->SetVertexData(_vertexes.data(), recoorded * 2);
		_mesh->SetIndexData(_indexes.data(), recoorded * 2);
	}

	void TrailRenderer::DrawImGui(TrailRenderer& component)
	{
		ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&component.Color));

		ImGui::DragFloat3("Base", reinterpret_cast<float*>(&component.base));
		ImGui::DragFloat3("Tip", reinterpret_cast<float*>(&component.tip));

		ImGui::DragFloat("RecoordFreq", &component.recoordFreq);
		ImGui::DragInt("MaxRecoord", reinterpret_cast<int*>(&component.maxRecoord));

		auto shaders = GET_SINGLE(Resources)->GetShaders();
		string skey;
		auto shader = component.material->GetShader();
		if (shader)
			skey = wtos(shader->GetKey());
		const char* currentShader = shader == nullptr ? "SpriteFoward" : skey.c_str();
		if (ImGui::BeginCombo("Shader", currentShader))
		{
			for (int i = 0; i < shaders.size(); i++)
			{
				string s = wtos(shaders[i]->GetKey());
				const char* currentShaderKey = s.c_str();
				bool isSelected = strcmp(currentShader, currentShaderKey) == 0;
				if (ImGui::Selectable(currentShaderKey, isSelected))
					component.material->SetShader(shaders[i]);

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Text("Texture", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath(path);
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texturePath.wstring(), texturePath.wstring(), false);
				assert(texture->GetD3Texture());
				component.material->SetTexture(0, texture);
			}
			ImGui::EndDragDropTarget();
		}

		auto tex = component.material->GetTexture(0);
		if (tex) {
			ImGui::Text(wtos(tex->GetPath()).c_str());

			if (ImGui::Button("Remove Diffuse"))
				component.material->SetTexture(0, nullptr);
		}
	}
	void LineRenderer::DrawImGui(LineRenderer& component)
	{
		ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&component.Color));
		ImGui::ColorEdit4("Emission", reinterpret_cast<float*>(&component.Emission), ImGuiColorEditFlags_::ImGuiColorEditFlags_HDR);

		ImGui::DragFloat2("Velocity", reinterpret_cast<float*>(&component.Velocity));
		ImGui::DragFloat2("Size", reinterpret_cast<float*>(&component.Size));
	}
}
