#include "pch.h"
#include "RigidBodyComponent.h"

namespace SY {
	void Rigidbody2DComponent::DrawImGui(Rigidbody2DComponent& component)
	{
		const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
		const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
		if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
				if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
				{
					currentBodyTypeString = bodyTypeStrings[i];
					component.Type = (Rigidbody2DComponent::BodyType)i;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
	}
}
