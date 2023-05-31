#include "pch.h"

#include "Light.h"
#include "SceneHierarchyPanel.h"
#include "Scene.h"

namespace SY {
	extern void DrawVec3Control(const std::string& label, Vector3& values, float resetValue, float columnWidth);

	void Light::DrawImGui(Light& component)
	{
		DrawVec3Control("Dir", component.info.dir, 0, 100);
		ImGui::ColorEdit3("Light Color", reinterpret_cast<float*>(&component.info.color), ImGuiColorEditFlags_::ImGuiColorEditFlags_HDR);
		ImGui::InputFloat("Light Range", &component.info.range);

		const char* LightTypes[] = { "Point", "Spot", "Directional", "Ambient" };

		if (ImGui::BeginCombo("State", LightTypes[(int)component.info.type]))
		{
			for (int i = 0; i < 4; i++)
			{
				bool is_selected = (int)component.info.type == i;
				if (ImGui::Selectable(LightTypes[i], is_selected))
					component.info.type = i;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
}
