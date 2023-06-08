#pragma once

namespace SY
{
	struct TransformComponent
	{
		Vector3 translation;
		Vector3 scale;
		Vector3 rotation;
		Matrix localToParent;
		Matrix localToWorld;

		bool recent = false;

		TransformComponent() : scale(Vector3::One), rotation(Vector3::Zero), translation(Vector3::Zero) {}
		TransformComponent(const TransformComponent&) = default;

		void CreateToParent()
		{
			if (!recent) {
				Matrix matScale = Matrix::CreateScale(scale);
				Matrix matRotation = Matrix::CreateFromYawPitchRoll(rotation);
				Matrix matTranslation = Matrix::CreateTranslation(translation);
				recent = true;
				localToParent = matScale * matRotation * matTranslation;
			}
		}

		static void DrawImGui(TransformComponent& component);

		void Serialize(YAML::Emitter& out);
		void Deserialize(YAML::Node& data);
	};

	struct RectTransformComponent
	{
		Vector3 scale;
		Vector3 translation;
		Vector3 worldTranslation;
		Matrix localToWorld;

		RectTransformComponent() : scale(Vector3::One), translation(Vector3::Zero) {}

		static void DrawImGui(RectTransformComponent& component);

		void Serialize(YAML::Emitter& out);
		void Deserialize(YAML::Node& data);
		void CreatToWorld();
	};
}
