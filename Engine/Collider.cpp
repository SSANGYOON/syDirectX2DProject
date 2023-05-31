#include "pch.h"
#include "Collider.h"
#include "Entity.h"

#include "box2d/b2_distance_joint.h"

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
		ImGui::Checkbox("Circle Collider : trigger", &component.isSensor);
	}
	void RevoluteJointComponent::DrawImGui(RevoluteJointComponent& component)
	{
		ImGui::Text("Opponent");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity data"))
				component.Opponent = *(UUID*)payload->Data;

			ImGui::EndDragDropTarget();
		}

		if (component.Opponent)
			ImGui::Text(to_string(component.Opponent).c_str());

		ImGui::DragFloat2("Local Anchor", reinterpret_cast<float*>(&component.LocalAnchor));
		ImGui::DragFloat2("Opponent Local Anchor", reinterpret_cast<float*>(&component.OpponentLocalAnchor));

		ImGui::Checkbox("Enable Angle Limit", &component.EnableLimit);

		if (component.EnableLimit)
			ImGui::DragFloat2("Angle Limit", reinterpret_cast<float*>(&component.AngleRange));
	}
	float DistanceJointComponent::GetCurrentLength()
	{
		b2DistanceJoint* dj = static_cast<b2DistanceJoint*>(b2Joint);
		return dj->GetCurrentLength();
	}
	void DistanceJointComponent::SetMinLength(float minLength)
	{
		b2DistanceJoint* dj = static_cast<b2DistanceJoint*>(b2Joint);
		dj->SetMinLength(minLength);
	}
	float DistanceJointComponent::GetMinLength()
	{
		b2DistanceJoint* dj = static_cast<b2DistanceJoint*>(b2Joint);
		return dj->GetMinLength();
	}
	void DistanceJointComponent::DrawImGui(DistanceJointComponent& component)
	{
		ImGui::Text("Opponent");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity data"))
				component.Opponent = *(UUID*)payload->Data;

			ImGui::EndDragDropTarget();
		}
		if (component.Opponent)
			ImGui::Text(to_string(component.Opponent).c_str());

		ImGui::DragFloat2("Local Anchor", reinterpret_cast<float*>(&component.LocalAnchor));
		ImGui::DragFloat2("Opponent Local Anchor", reinterpret_cast<float*>(&component.OpponentLocalAnchor));

		ImGui::DragFloat("MinLength", &component.minLength, 0.f);
		ImGui::DragFloat("MaxLength", &component.maxLength, 0.f);

		ImGui::DragFloat("Stiffness", &component.stiffness, 0.f);
		ImGui::DragFloat("Damping", &component.damping, 0.f);
	}
}