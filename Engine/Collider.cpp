#include "pch.h"
#include "Collider.h"

namespace SY {
	void BoxCollider2DComponent::DrawImGui(BoxCollider2DComponent& component)
	{
		ImGui::DragFloat2("Offset", reinterpret_cast<float*>(&component.Offset));
		ImGui::DragFloat2("Size", reinterpret_cast<float*>(&component.Size));
		ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);

		bool categoryBits[8];
		int category = component.categoryBits;
		for (int i = 0; i < 8; i++)
		{
			categoryBits[i] = category % 2;
			category /= 2;
		}

		bool maskBits[8];
		int mask = component.maskBits;
		for (int i = 0; i < 8; i++)
		{
			maskBits[i] = mask % 2;
			mask /= 2;
		}

		ImGui::Text("Category");
		for (int i = 0; i < 8; i++) {
			ImGui::Checkbox(("C" + to_string(i)).c_str(), &categoryBits[i]);
			if (i < 7)
				ImGui::SameLine();
		}

		ImGui::Text("maskBits");
		for (int i = 0; i < 8; i++) {
			ImGui::Checkbox(("M" + to_string(i)).c_str(), &maskBits[i]);
			if (i < 7)
				ImGui::SameLine();
		}

		component.categoryBits = 0;
		for (int i = 0; i < 8; i++)
		{
			component.categoryBits += categoryBits[i] * (1 << i);
		}
		component.maskBits = 0;
		for (int i = 0; i < 8; i++)
		{
			component.maskBits += maskBits[i] * (1 << i);
		}
		ImGui::Checkbox("Box Collider : trigger", &component.isSensor);
	}
	void CircleCollider2DComponent::DrawImGui(CircleCollider2DComponent& component)
	{
		ImGui::DragFloat2("Offset", reinterpret_cast<float*>(&component.Offset));
		ImGui::DragFloat("Radius", &component.Radius);
		ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);

		bool categoryBits[8];
		int category = component.categoryBits;
		for (int i = 0; i < 8; i++)
		{
			categoryBits[i] = category % 2;
			category /= 2;
		}

		bool maskBits[8];
		int mask = component.maskBits;
		for (int i = 0; i < 8; i++)
		{
			maskBits[i] = mask % 2;
			mask /= 2;
		}

		ImGui::Text("Category");
		for (int i = 0; i < 8; i++) {
			ImGui::Checkbox(to_string(i).c_str(), &categoryBits[i]);
			if (i < 7)
				ImGui::SameLine();
		}

		ImGui::Text("maskBits");
		for (int i = 0; i < 8; i++) {
			ImGui::Checkbox(to_string(i).c_str(), &maskBits[i]);
			if (i < 7)
				ImGui::SameLine();
		}

		component.categoryBits = 0;
		for (int i = 0; i < 8; i++)
		{
			component.categoryBits += categoryBits[i] * (1 << i);
		}
		component.maskBits = 0;
		for (int i = 0; i < 8; i++)
		{
			component.maskBits += maskBits[i] * (1 << i);
		}
		ImGui::Checkbox("Circle Collider : trigger", &component.isSensor);
	}
}