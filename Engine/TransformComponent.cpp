#include "pch.h"
#include "TransformComponent.h"
#include "SceneHierarchyPanel.h"
#include "SerializerUtils.h"

namespace SY {
	extern void DrawVec3Control(const std::string& label, Vector3& values, float resetValue, float columnWidth);
	void TransformComponent::DrawImGui(TransformComponent& component)
	{
		DrawVec3Control("Translation", component.translation,0,100);
		Vector3 rotation = component.rotation * 180.f / XM_PI;
		DrawVec3Control("Rotation", rotation,0,100);
		component.rotation = rotation / 180.f * XM_PI;
		DrawVec3Control("Scale", component.scale, 1.0f,100);

		component.recent = false;
	}
	void TransformComponent::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap;


		out << YAML::Key << "Translation" << YAML::Value << translation;
		out << YAML::Key << "Rotation" << YAML::Value << rotation;
		out << YAML::Key << "Scale" << YAML::Value << scale;

		out << YAML::EndMap;
	}

	void TransformComponent::Deserialize(YAML::Node& data)
	{
		translation = data["Translation"].as<Vector3>();
		rotation = data["Rotation"].as<Vector3>();
		scale = data["Scale"].as<Vector3>();
	}

	
	void RectTransformComponent::DrawImGui(RectTransformComponent& component)
	{
		DrawVec3Control("Translation", component.translation, 0, 100);
		DrawVec3Control("Scale", component.scale, 0, 100);
	}
	void RectTransformComponent::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "RectTransformComponent";
		out << YAML::BeginMap;


		out << YAML::Key << "Translation" << YAML::Value << translation;
		out << YAML::Key << "Scale" << YAML::Value << scale;

		out << YAML::EndMap;	
	}
	void RectTransformComponent::Deserialize(YAML::Node& data)
	{
		translation = data["Translation"].as<Vector3>();
		scale = data["Scale"].as<Vector3>();
	}
	void RectTransformComponent::CreatToWorld()
	{
		Matrix matScale = Matrix::CreateScale(scale);
		Matrix matTranslation = Matrix::CreateTranslation(worldTranslation);
		localToWorld = matScale * matTranslation;
	}
}
