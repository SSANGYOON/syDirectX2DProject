#include "pch.h"
#include "Resources.h"
#include "SceneSerializer.h"
#include "Scene.h"
#include "Entity.h"
#include "ScriptEngine.h"

#include "Project.h"

#include "SerializerUtils.h"
#include "Animation.h"
#include "Texture.h"
#include <bitset>

namespace YAML
{
	template<>
	struct convert<SY::UUID>
	{
		static Node encode(const SY::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, SY::UUID& uuid)
		{
			uuid = node.as<uint64_t>();
			return true;
		}
	};
}

namespace SY {

#define WRITE_SCRIPT_FIELD(FieldType, Type)           \
			case ScriptFieldType::FieldType:          \
				out << scriptField.GetValue<Type>();  \
				break

#define READ_SCRIPT_FIELD(FieldType, Type)             \
	case ScriptFieldType::FieldType:                   \
	{                                                  \
		Type data = scriptField["Data"].as<Type>();    \
		fieldInstance.SetValue(data);                  \
		break;                                         \
	}

	static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Rigidbody2DComponent::BodyType::Static:    return "Static";
		case Rigidbody2DComponent::BodyType::Dynamic:   return "Dynamic";
		case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		assert(false);
		return {};
	}

	static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static")    return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic")   return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

		assert(false);
		return Rigidbody2DComponent::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const shared_ptr<Scene>& scene)
		: m_Scene(scene)
	{
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		assert(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<StateComponent>())
		{
			auto& state = entity.GetComponent<StateComponent>();
			out << YAML::Key << "State";
			out << YAML::Value << (UINT)state.state;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			auto& tc = entity.GetComponent<TransformComponent>();
			tc.Serialize(out);
		}

		if (entity.HasComponent<RectTransformComponent>())
		{
			auto& tc = entity.GetComponent<RectTransformComponent>();
			tc.Serialize(out);
		}

		if (entity.HasComponent<Parent>())
		{
			out << YAML::Key << "Parent";
			out << YAML::BeginMap; // TransformComponent

			auto& pc = entity.GetComponent<Parent>();
			out << YAML::Key << "ParentID" << YAML::Value << pc.parentHandle;

			out << YAML::EndMap; // Parent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "Near" << YAML::Value << camera.GetNearClip();
			out << YAML::Key << "Far" << YAML::Value << camera.GetFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();

			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;
			out << YAML::Key << "LayerMask" << YAML::Value << cameraComponent.LayerBit;

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.HasComponent<BackGroundColorComponent>())
		{
			auto& background = entity.GetComponent<BackGroundColorComponent>();
			out << YAML::Key << "BackGroundColorComponent";
			out << YAML::Value << background.color;
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			auto& scriptComponent = entity.GetComponent<ScriptComponent>();

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent
			out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;

			// Fields
			shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClass(scriptComponent.ClassName);
			const auto& fields = entityClass->GetFields();
			if (fields.size() > 0)
			{
				out << YAML::Key << "ScriptFields" << YAML::Value;
				auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
				out << YAML::BeginSeq;
				for (const auto& [name, field] : fields)
				{
					if (entityFields.find(name) == entityFields.end())
						continue;

					out << YAML::BeginMap; // ScriptField
					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);

					out << YAML::Key << "Data" << YAML::Value;
					ScriptFieldInstance& scriptField = entityFields.at(name);
					switch (field.Type)
					{
						WRITE_SCRIPT_FIELD(Float, float);
						WRITE_SCRIPT_FIELD(Double, double);
						WRITE_SCRIPT_FIELD(Bool, bool);
						WRITE_SCRIPT_FIELD(Char, char);
						WRITE_SCRIPT_FIELD(Byte, int8_t);
						WRITE_SCRIPT_FIELD(Short, int16_t);
						WRITE_SCRIPT_FIELD(Int, int32_t);
						WRITE_SCRIPT_FIELD(Long, int64_t);
						WRITE_SCRIPT_FIELD(UByte, uint8_t);
						WRITE_SCRIPT_FIELD(UShort, uint16_t);
						WRITE_SCRIPT_FIELD(UInt, uint32_t);
						WRITE_SCRIPT_FIELD(ULong, uint64_t);
						WRITE_SCRIPT_FIELD(Vector2, Vector2);
						WRITE_SCRIPT_FIELD(Vector3, Vector3);
						WRITE_SCRIPT_FIELD(Vector4, Vector4);
						WRITE_SCRIPT_FIELD(Entity, UUID);
						case ScriptFieldType::String:
						{
							out << string(scriptField.GetString());
							break;
						}
					}
					out << YAML::EndMap; // ScriptFields
				}
				out << YAML::EndSeq;
			}

			out << YAML::EndMap; // ScriptComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();

			if (spriteRendererComponent.shader)
				out << YAML::Key << "Shader" << YAML::Value << wtos(spriteRendererComponent.shader->GetKey());

			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
			out << YAML::Key << "Emission" << YAML::Value << spriteRendererComponent.Emission;
			if (spriteRendererComponent.Diffuse) {
				wstring wpath = spriteRendererComponent.Diffuse->GetPath();

				out << YAML::Key << "DiffusePath" << YAML::Value << wtos(wpath);
				out << YAML::Key << "Tile" << YAML::Value << spriteRendererComponent.tile;
				out << YAML::Key << "Offset" << YAML::Value << spriteRendererComponent.Offset;
				out << YAML::Key << "Layer" << YAML::Value << spriteRendererComponent.LayerBit;
				out << YAML::Key << "Deffered" << YAML::Value << spriteRendererComponent.Deffered;
			}

			if (spriteRendererComponent.LightMap)
			{
				wstring wpath = spriteRendererComponent.LightMap->GetPath();
				out << YAML::Key << "LightMapPath" << YAML::Value << wtos(wpath);
			}

			for (int i = 0; i < 4; i++)
			{
				string label = "int" + to_string(i);
				out << YAML::Key << label << YAML::Value << spriteRendererComponent.material->GetInt(i);
			}

			for (int i = 0; i < 4; i++)
			{
				string label = "float" + to_string(i);
				out << YAML::Key << label << YAML::Value << spriteRendererComponent.material->GetFloat(i);
			}

			for (int i = 0; i < 4; i++)
			{
				string label = "vec2" + to_string(i);
				out << YAML::Key << label << YAML::Value << spriteRendererComponent.material->GetVec2(i);
			}

			for (int i = 0; i < 4; i++)
			{
				string label = "vec4" + to_string(i);
				out << YAML::Key << label << YAML::Value << spriteRendererComponent.material->GetVec4(i);
			}

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<Eraser>())
		{
			out << YAML::Key << "Eraser";
			out << YAML::BeginMap; // Eraser

			out << YAML::EndMap; // Eraser
		}

		if (entity.HasComponent<Light>())
		{
			out << YAML::Key << "Light";
			out << YAML::BeginMap;

			auto& light = entity.GetComponent<Light>();
			out << YAML::Key << "Color" << YAML::Value << light.info.color;
			out << YAML::Key << "Type" << YAML::Value << light.info.type;
			out << YAML::Key << "Range" << YAML::Value << light.info.range;
			out << YAML::Key << "Direction" << YAML::Value << light.info.dir;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;
			out << YAML::Key << "LinearDamping" << YAML::Value << rb2dComponent.LinearDamping;
			out << YAML::Key << "AngularDamping" << YAML::Value << rb2dComponent.AngularDamping;
			out << YAML::Key << "DisableGravity" << YAML::Value << rb2dComponent.DisableGravity;

			out << YAML::EndMap; // Rigidbody2DComponent
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;
			out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;
			out << YAML::Key << "IsSensor" << YAML::Value << bc2dComponent.isSensor;
			out << YAML::Key << "MaskBits" << YAML::Value << bc2dComponent.maskBits;
			out << YAML::Key << "CategoryBits" << YAML::Value << bc2dComponent.categoryBits;
			out << YAML::EndMap; // BoxCollider2DComponent
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent

			auto& cc2dComponent = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.Offset;
			out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.Radius;
			out << YAML::Key << "Density" << YAML::Value << cc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2dComponent.RestitutionThreshold;
			out << YAML::Key << "IsSensor" << YAML::Value << cc2dComponent.isSensor;
			out << YAML::Key << "MaskBits" << YAML::Value << cc2dComponent.maskBits;
			out << YAML::Key << "CategoryBits" << YAML::Value << cc2dComponent.categoryBits;

			out << YAML::EndMap; // CircleCollider2DComponent
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent

			auto& cr = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << cr.Color;
			out << YAML::EndMap; // CircleRendererComponent
		}

		if (entity.HasComponent<RevoluteJointComponent>())
		{
			out << YAML::Key << "RevoluteJointComponent";
			out << YAML::BeginMap; // RevoluteJointComponent

			auto& joint = entity.GetComponent<RevoluteJointComponent>();
			out << YAML::Key << "Opponent" << YAML::Value << joint.Opponent;
			out << YAML::Key << "LocalAnchor" << YAML::Value << joint.LocalAnchor;
			out << YAML::Key << "OpponentLocalAnchor" << YAML::Value << joint.OpponentLocalAnchor;
			out << YAML::Key << "EnableLimit" << YAML::Value << joint.EnableLimit;
			out << YAML::Key << "AngleRange" << YAML::Value << joint.AngleRange;

			out << YAML::EndMap; // RevoluteJointComponent
		}

		if (entity.HasComponent<DistanceJointComponent>())
		{
			out << YAML::Key << "DistanceJointComponent";
			out << YAML::BeginMap; // DistanceJointComponent

			auto& joint = entity.GetComponent<DistanceJointComponent>();
			out << YAML::Key << "Opponent" << YAML::Value << joint.Opponent;
			out << YAML::Key << "LocalAnchor" << YAML::Value << joint.LocalAnchor;
			out << YAML::Key << "OpponentLocalAnchor" << YAML::Value << joint.OpponentLocalAnchor;
			out << YAML::Key << "minLength" << YAML::Value << joint.minLength;
			out << YAML::Key << "maxLength" << YAML::Value << joint.maxLength;

			out << YAML::Key << "stiffness" << YAML::Value << joint.stiffness;
			out << YAML::Key << "damping" << YAML::Value << joint.damping;

			out << YAML::EndMap; // DistanceJointComponent
		}

		if (entity.HasComponent<SpriteAnimatorComponent>())
		{
			out << YAML::Key << "AnimatorComponent";
			out << YAML::BeginMap; // AnimatorComponent

			auto& animator = entity.GetComponent<SpriteAnimatorComponent>();
			if(animator.clips.size())
				out << YAML::Key << "ClipPath" << YAML::Value << wtos(animator.clips.begin()->second->GetPath());

			out << YAML::Key << "StartEvents";
			out << YAML::BeginSeq;
			for (auto event : animator._startEvent)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "key" << YAML::Value << event.first;
				out << YAML::Key << "function" << YAML::Value << event.second;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;

			out << YAML::Key << "EndEvents";
			out << YAML::BeginSeq;
			for (auto event : animator._endEvent)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "key" << YAML::Value << event.first;
				out << YAML::Key << "function" << YAML::Value << event.second;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;	

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformAnimatorComponent>())
		{
			out << YAML::Key << "TransfromAnimatorComponent";
			out << YAML::BeginMap;

			auto& animator = entity.GetComponent<TransformAnimatorComponent>();
			if (animator.clips.size())
				out << YAML::Key << "ClipPath" << YAML::Value << wtos(animator.clips.begin()->second->GetPath());

			out << YAML::EndMap;
		}

		if (entity.HasComponent<PanelComponent>())
		{
			out << YAML::Key << "PanelComponent";
			out << YAML::BeginMap; // PanelComponent

			auto& panel = entity.GetComponent<PanelComponent>();
			if (panel.panel) {
				wstring wpath = panel.panel->GetPath();

				out << YAML::Key << "PanelPath" << YAML::Value << wtos(wpath);
				
			}
			out << YAML::Key << "resizeTexture" << YAML::Value << panel.resizeTexture;
			out << YAML::Key << "panelOffset" << YAML::Value << panel.offset;
			out << YAML::Key << "tineColor" << YAML::Value << panel.color;
			out << YAML::Key << "tineRange" << YAML::Value << panel.tintOffset;
			out << YAML::EndMap; // PanelComponent
		}

		if (entity.HasComponent<SliderComponent>())
		{
			out << YAML::Key << "SliderComponent";
			out << YAML::BeginMap; // SliderComponent

			auto& slider = entity.GetComponent<SliderComponent>();
			if (slider.bar && slider.gauge) {
				wstring barPath = slider.bar->GetPath();
				wstring gaugePath = slider.gauge->GetPath();
				out << YAML::Key << "barPath" << YAML::Value << wtos(barPath);
				out << YAML::Key << "gaugePath" << YAML::Value << wtos(gaugePath);
				
			}

			out << YAML::Key << "showNumeric" << YAML::Value << slider.showNumeric;
			out << YAML::Key << "currentValue" << YAML::Value << slider.currentValue;
			out << YAML::Key << "maxValue" << YAML::Value << slider.maxValue;
			out << YAML::EndMap; // SliderComponent
		}

		if (entity.HasComponent<SlotComponent>())
		{
			out << YAML::Key << "SlotComponent";
			out << YAML::BeginMap; // SlotComponent

			auto& slot = entity.GetComponent<SlotComponent>();

			out << YAML::Key << "Rotation" << YAML::Value << slot.rotation;
			out << YAML::Key << "ItemSize" << YAML::Value << slot.itemSize;
			
			if (slot.slot) {
				wstring slotPath = slot.slot->GetPath();
				out << YAML::Key << "slotPath" << YAML::Value << wtos(slotPath);
			}
			if (slot.slotMask) {
				wstring itemPath = slot.slotMask->GetPath();
				out << YAML::Key << "maskPath" << YAML::Value << wtos(itemPath);
			}
			if (slot.item) {
				wstring itemPath = slot.item->GetPath();
				out << YAML::Key << "itemPath" << YAML::Value << wtos(itemPath);
			}

			out << YAML::EndMap; // SlotComponent
		}

		if (entity.HasComponent<IconComponent>())
		{
			out << YAML::Key << "IconComponent";
			out << YAML::BeginMap;

			auto& icon = entity.GetComponent<IconComponent>();

			out << YAML::Key << "tint" << YAML::Value << icon.tint;

			if (icon.icon) {
				wstring iconPath = icon.icon->GetPath();
				out << YAML::Key << "iconPath" << YAML::Value << wtos(iconPath);
			}

			out << YAML::EndMap; // SlotComponent
		}

		if (entity.HasComponent<ParticleSystem>())
		{
			out << YAML::Key << "ParticleSystem";
			out << YAML::BeginMap;

			auto& particle = entity.GetComponent<ParticleSystem>();

			out << YAML::Key << "Position" << YAML::Value << particle.Position;
			out << YAML::Key << "PositionVariation" << YAML::Value << particle.PositionVariation;

			out << YAML::Key << "Velocity" << YAML::Value << particle.Velocity;
			out << YAML::Key << "VelocityVariation" << YAML::Value << particle.VelocityVariation;
			out << YAML::Key << "VelocityEnd" << YAML::Value << particle.VelocityEnd;

			out << YAML::Key << "SizeBegin" << YAML::Value << particle.SizeBegin;
			out << YAML::Key << "SizeEnd" << YAML::Value << particle.SizeEnd;
			out << YAML::Key << "SizeVariation" << YAML::Value << particle.SizeVariation;

			out << YAML::Key << "ColorBegin" << YAML::Value << particle.ColorBegin;
			out << YAML::Key << "ColorEnd" << YAML::Value << particle.ColorEnd;

			out << YAML::Key << "EmissionBegin" << YAML::Value << particle.EmissionBegin;
			out << YAML::Key << "EmissionEnd" << YAML::Value << particle.EmissionEnd;

			out << YAML::Key << "MaxParticles" << YAML::Value << particle.MaxParticles;
			out << YAML::Key << "Frequency" << YAML::Value << particle.Frequency;
			out << YAML::Key << "LifeTime" << YAML::Value << particle.LifeTime;

			out << YAML::Key << "PositionPolar" << YAML::Value << particle.PositionPolar;
			out << YAML::Key << "VelocityPolar" << YAML::Value << particle.VelocityPolar;

			out << YAML::Key << "UseLocalCoord" << YAML::Value << particle.UseLocalCoord;
			out << YAML::Key << "UseAliveZone" << YAML::Value << particle.UseAliveZone;

			if(particle.UseAliveZone)
				out << YAML::Key << "AliveZone" << YAML::Value << particle.aliveZone;

			out << YAML::Key << "State" << YAML::Value << (UINT)particle.state;

			if (particle.attachTexture) {
				wstring path = particle.attachTexture->GetPath();
				out << YAML::Key << "AttachTexture" << YAML::Value << wtos(path);

				out << YAML::Key << "AttachOffset" << YAML::Value << particle.AttachOffset;
			}

			if (particle.graphicsTexture) {
				wstring path = particle.graphicsTexture->GetPath();
				out << YAML::Key << "GraphicsTexture" << YAML::Value << wtos(path);
			}

			wstring key = particle.mMaterial->GetShader()->GetKey();
			out << YAML::Key << "GraphicsShader" << YAML::Value << wtos(key);

			out << YAML::EndMap; // ParticleSystem
		}

		if (entity.HasComponent<Bloom>())
		{
			out << YAML::Key << "Bloom";
			out << YAML::BeginMap;

			auto& bloom = entity.GetComponent<Bloom>();

			out << YAML::Key << "Threshold" << YAML::Value << bloom.Threshold;
			out << YAML::Key << "Intensity" << YAML::Value << bloom.Intensity;

			out << YAML::EndMap; // Bloom
		}

		if (entity.HasComponent<TrailRenderer>())
		{
			out << YAML::Key << "TrailRenderer";
			out << YAML::BeginMap;

			auto& trail = entity.GetComponent<TrailRenderer>();

			out << YAML::Key << "Color" << YAML::Value << trail.Color;
			out << YAML::Key << "Base" << YAML::Value << trail.base;
			out << YAML::Key << "Tip" << YAML::Value << trail.tip;

			wstring shaderKey = trail.material->GetShader()->GetKey();
			out << YAML::Key << "Shader" << YAML::Value << wtos(shaderKey);

			wstring texturePath = trail.material->GetTexture(0)->GetPath();
			out << YAML::Key << "Texture" << YAML::Value << wtos(texturePath);

			out << YAML::EndMap; // TrailRenderer
		}

		if (entity.HasComponent<LineRenderer>())
		{
			out << YAML::Key << "LineRenderer";
			out << YAML::BeginMap;

			auto& line = entity.GetComponent<LineRenderer>();

			out << YAML::Key << "Color" << YAML::Value << line.Color;
			out << YAML::Key << "Emission" << YAML::Value << line.Emission;

			out << YAML::Key << "Velocity" << YAML::Value << line.Velocity;
			out << YAML::Key << "Size" << YAML::Value << line.Size;

			out << YAML::EndMap; // LineRenderer
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		// Not implemented
		assert(false);
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			assert(false);
			//HZ_CORE_ERROR("Failed to load .hazel file '{0}'\n     {1}", filepath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		
		//HZ_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				DeserializeEntity(m_Scene.get(), entity);
			}
		}

		return true;
	}

	Entity SceneSerializer::DeserializeEntity(Scene* scene, YAML::Node& entity)
	{
		uint64_t uuid = entity["Entity"].as<uint64_t>();

		std::string name;
		auto tagComponent = entity["TagComponent"];
		if (tagComponent)
			name = tagComponent["Tag"].as<std::string>();

		Entity deserializedEntity = scene->CreateEntityWithUUID(uuid, name);

		auto stateComponent = entity["State"];
		if(stateComponent)
		{
			auto& state = deserializedEntity.GetComponent<StateComponent>();
			state.state = (EntityState)stateComponent.as<UINT>();
		}

		auto transformComponent = entity["TransformComponent"];
		if (transformComponent)
		{
			auto& tc = deserializedEntity.AddComponent<TransformComponent>();
			tc.Deserialize(transformComponent);
		}

		auto rectTransformComponent = entity["RectTransformComponent"];
		if (rectTransformComponent)
		{
			auto& tc = deserializedEntity.AddComponent<RectTransformComponent>();
			tc.Deserialize(rectTransformComponent);
		}

		auto parent = entity["Parent"];
		if (parent)
		{
			auto& pc = deserializedEntity.AddComponent<Parent>();
			pc.parentHandle = parent["ParentID"].as<uint64_t>();
		}

		auto scriptComponent = entity["ScriptComponent"];
		if (scriptComponent)
		{
			auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
			sc.ClassName = scriptComponent["ClassName"].as<std::string>();

			auto scriptFields = scriptComponent["ScriptFields"];
			if (scriptFields)
			{
				shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
				if (entityClass)
				{
					const auto& fields = entityClass->GetFields();
					auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

					for (auto scriptField : scriptFields)
					{
						std::string name = scriptField["Name"].as<std::string>();
						std::string typeString = scriptField["Type"].as<std::string>();
						ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

						ScriptFieldInstance& fieldInstance = entityFields[name];

						// TODO(Yan): turn this assert into Hazelnut log warning
						assert(fields.find(name) != fields.end());

						if (fields.find(name) == fields.end())
							continue;

						fieldInstance.Field = fields.at(name);

						switch (type)
						{
							READ_SCRIPT_FIELD(Float, float);
							READ_SCRIPT_FIELD(Double, double);
							READ_SCRIPT_FIELD(Bool, bool);
							READ_SCRIPT_FIELD(Char, char);
							READ_SCRIPT_FIELD(Byte, int8_t);
							READ_SCRIPT_FIELD(Short, int16_t);
							READ_SCRIPT_FIELD(Int, int32_t);
							READ_SCRIPT_FIELD(Long, int64_t);
							READ_SCRIPT_FIELD(UByte, uint8_t);
							READ_SCRIPT_FIELD(UShort, uint16_t);
							READ_SCRIPT_FIELD(UInt, uint32_t);
							READ_SCRIPT_FIELD(ULong, uint64_t);
							READ_SCRIPT_FIELD(Vector2, Vector2);
							READ_SCRIPT_FIELD(Vector3, Vector3);
							READ_SCRIPT_FIELD(Vector4, Vector4);
							READ_SCRIPT_FIELD(Entity, UUID);

							case ScriptFieldType::String:
							{
								string data = scriptField["Data"].as<string>();
								fieldInstance.SetString(data.c_str());
								break;
							}
						}
					}
				}
			}
		}

		auto cameraComponent = entity["CameraComponent"];
		if (cameraComponent)
		{
			auto& cc = deserializedEntity.AddComponent<CameraComponent>();

			auto cameraProps = cameraComponent["Camera"];
			cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

			cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
			cc.Camera.SetNearClip(cameraProps["Near"].as<float>());
			cc.Camera.SetFarClip(cameraProps["Far"].as<float>());

			cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());

			cc.Primary = cameraComponent["Primary"].as<bool>();
			cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			if(cameraComponent["LayerMask"])
				cc.LayerBit  = cameraComponent["LayerMask"].as<UINT16>();
		}

		auto backgroundcolorComponent = entity["BackGroundColorComponent"];
		if (backgroundcolorComponent)
		{
			auto& background = deserializedEntity.AddComponent<BackGroundColorComponent>();
			background.color = backgroundcolorComponent.as<Vector4>();
		}

		auto spriteRendererComponent = entity["SpriteRendererComponent"];
		if (spriteRendererComponent)
		{
			auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
			src.Color = spriteRendererComponent["Color"].as<Vector4>();

			if(spriteRendererComponent["Emission"])
				src.Emission = spriteRendererComponent["Emission"].as<Vector4>();

			if (spriteRendererComponent["Shader"])
				src.shader = GET_SINGLE(Resources)->Find<Shader>(stow(spriteRendererComponent["Shader"].as<string>()));

			if (spriteRendererComponent["DiffusePath"])
			{
				std::string texturePath = spriteRendererComponent["DiffusePath"].as<std::string>();
				auto path = Project::GetAssetFileSystemPath(texturePath);
				src.Diffuse = GET_SINGLE(Resources)->Load<Texture>(path.wstring(), path.wstring(), false);
				if(spriteRendererComponent["Offset"])
					src.Offset = spriteRendererComponent["Offset"].as<Vector2>();
				if (spriteRendererComponent["Offset"])
					src.tile = spriteRendererComponent["Tile"].as<Vector2>();
				if(spriteRendererComponent["Deffered"])
					src.Deffered = spriteRendererComponent["Deffered"].as<bool>();
			}

			if (spriteRendererComponent["LightMapPath"])
			{
				std::string texturePath = spriteRendererComponent["LightMapPath"].as<std::string>();
				auto path = Project::GetAssetFileSystemPath(texturePath);
				src.LightMap = GET_SINGLE(Resources)->Load<Texture>(path.wstring(), path.wstring(), false);
			}

			if (spriteRendererComponent["Layer"])
				src.LayerBit = spriteRendererComponent["Layer"].as<UINT16>();

			for (int i = 0; i < 4; i++)
			{
				string label = "int" + to_string(i);
				if(spriteRendererComponent[label])
					src.material->SetInt(i, spriteRendererComponent[label].as<int>());
			}

			for (int i = 0; i < 4; i++)
			{
				string label = "float" + to_string(i);
				if (spriteRendererComponent[label])
					src.material->SetFloat(i, spriteRendererComponent[label].as<float>());
			}

			for (int i = 0; i < 4; i++)
			{
				string label = "vec2" + to_string(i);
				if (spriteRendererComponent[label])
					src.material->SetVec2(i, spriteRendererComponent[label].as<Vector2>());
			}

			for (int i = 0; i < 4; i++)
			{
				string label = "vec4" + to_string(i);
				if (spriteRendererComponent[label])
					src.material->SetVec4(i, spriteRendererComponent[label].as<Vector4>());
			}
		}

		auto circleRendererComponent = entity["CircleRendererComponent"];
		if (circleRendererComponent)
		{
			auto& cr = deserializedEntity.AddComponent<CircleRendererComponent>();
			cr.Color = circleRendererComponent["Color"].as<Vector4>();
		}


		auto eraser = entity["Eraser"];
		if (eraser)
			deserializedEntity.AddComponent<Eraser>();

		auto light = entity["Light"];
		if (light)
		{
			auto& lightComp = deserializedEntity.AddComponent<Light>();
			lightComp.info.color = light["Color"].as<Vector3>();
			lightComp.info.dir = light["Direction"].as<Vector3>();
			lightComp.info.type = light["Type"].as<UINT32>();
			lightComp.info.range = light["Range"].as<float>();

		}

		auto animatorComponent = entity["AnimatorComponent"];
		if (animatorComponent)
		{
			auto& animator = deserializedEntity.AddComponent<SpriteAnimatorComponent>();

			animator._currentTime = 0.f;
			animator._lastTime = -1.f;
			animator._endedAt = 0.f;
			animator._currentClip = nullptr;

			auto clipPath = animatorComponent["ClipPath"];

			if (clipPath) {
				YAML::Node data;

				string relativePath = clipPath.as<string>();

				std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
				auto resourcedir = parentPath / "Resources";

				auto fullPath = resourcedir / relativePath;

				try
				{
					data = YAML::LoadFile(fullPath.string());
				}
				catch (YAML::ParserException e)
				{
					assert(false);
				}

				assert(data["Animations"]);

				auto clips = data["Animations"];

				for (auto clip : clips)
				{
					auto Key = clip["Key"];
					auto OriginalOffset = clip["OriginalOffset"];
					auto SpriteSize = clip["SpriteSize"];
					auto Step = clip["Step"];
					auto Columns = clip["Columns"];
					auto Frames = clip["Frames"];
					auto Duration = clip["Duration"];
					auto nextKey = clip["nextKey"];
					auto loop = clip["Loop"];
					string key;
					key = Key.as<string>();
					shared_ptr<Animation> clipRef;
					if ((clipRef = GET_SINGLE(Resources)->Find<Animation>(stow(key))) == nullptr){
						Vector2 offset = OriginalOffset.as<Vector2>();
						Vector2 size = SpriteSize.as<Vector2>();
						Vector2 step = Step.as<Vector2>();
						UINT columns = Columns.as<UINT>();
						UINT frame = Frames.as<UINT>();
						float duration = Duration.as<float>();
						bool bloop = loop.as<bool>();
						Vector2 targetOffset = Vector2::Zero;
						if (clip["TargetOffset"])
							targetOffset = clip["TargetOffset"].as<Vector2>();

						string nextClipKey = "";
						if (nextKey)
							nextClipKey = nextKey.as<string>();
						clipRef = make_shared<Animation>();
						clipRef->Load(offset, size, step, targetOffset, columns, frame, duration, key, bloop, nextClipKey);
						clipRef->SetPath(stow(relativePath));
						GET_SINGLE(Resources)->Insert(stow(key), clipRef);
					}
					animator.clips[key] = clipRef;
				}
			}
			auto startEvents = animatorComponent["StartEvents"];
			for (auto event : startEvents)
			{
				string key = event["key"].as<string>();
				string function = event["function"].as<string>();

				animator._startEvent[key] = function;
			}
			auto endEvents = animatorComponent["EndEvents"];
			for (auto event : endEvents)
			{
				string key = event["key"].as<string>();
				string function = event["function"].as<string>();

				animator._endEvent[key] = function;
			}
		}

		auto transformanimatorComponent = entity["TransfromAnimatorComponent"];
		if (transformanimatorComponent)
		{
			auto& animator = deserializedEntity.AddComponent<TransformAnimatorComponent>();

			animator._currentTime = 0.f;
			animator._lastTime = -1.f;
			animator._endedAt = 0.f;
			animator._currentClip = nullptr;

			auto clipPath = transformanimatorComponent["ClipPath"];

			if (clipPath) {
				YAML::Node data;

				string relativePath = clipPath.as<string>();

				std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
				auto resourcedir = parentPath / "Resources";

				auto fullPath = resourcedir / relativePath;

				try
				{
					data = YAML::LoadFile(fullPath.string());
				}
				catch (YAML::ParserException e)
				{
					assert(false);
				}

				assert(data["Animations"]);
				auto clips = data["Animations"];

				for (auto clip : clips)
				{
					auto Key = clip["ClipName"];
					auto Frames = clip["Frames"];
					auto Duration = clip["Duration"];
					auto Loop = clip["Loop"];

					string key;
					key = Key.as<string>();
					shared_ptr<TransformAnimation> clipRef = GET_SINGLE(Resources)->Find<TransformAnimation>(stow(key));
					if (clipRef == nullptr) {
						clipRef = make_shared<TransformAnimation>();
						vector<TransformFrame> frames;
						for (auto frame : Frames)
						{
							TransformFrame tf;
							tf.position = frame["Offset"].as<Vector3>();
							tf.angle = frame["Angle"].as<float>();
							frames.push_back(tf);
						}
						clipRef->Load(key, Duration.as<float>(), Loop.as<bool>(), frames);
					}

					std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
					auto resourcedir = parentPath / "Resources";
					clipRef->SetPath(filesystem::relative(fullPath, resourcedir).wstring());
					animator.clips[key] = clipRef;
				}
			}
		}

		auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
		if (rigidbody2DComponent)
		{
			auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
			rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
			rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
			if(rigidbody2DComponent["LinearDamping"])
				rb2d.LinearDamping = rigidbody2DComponent["LinearDamping"].as<float>();
			if(rigidbody2DComponent["AngularDamping"])
				rb2d.AngularDamping = rigidbody2DComponent["AngularDamping"].as<float>();
			if (rigidbody2DComponent["DisableGravity"])
				rb2d.DisableGravity = rigidbody2DComponent["DisableGravity"].as<bool>();
		}

		auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
		if (boxCollider2DComponent)
		{
			auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
			bc2d.Offset = boxCollider2DComponent["Offset"].as<Vector2>();
			bc2d.Size = boxCollider2DComponent["Size"].as<Vector2>();
			bc2d.Density = boxCollider2DComponent["Density"].as<float>();
			bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
			bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
			bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
			bc2d.isSensor = boxCollider2DComponent["IsSensor"].as<bool>();
			if(boxCollider2DComponent["MaskBits"])
				bc2d.maskBits = boxCollider2DComponent["MaskBits"].as<UINT16>();
			if (boxCollider2DComponent["CategoryBits"])
				bc2d.categoryBits = boxCollider2DComponent["CategoryBits"].as<UINT16>();
		}

		auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
		if (circleCollider2DComponent)
		{
			auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
			cc2d.Offset = circleCollider2DComponent["Offset"].as<Vector2>();
			cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
			cc2d.Density = circleCollider2DComponent["Density"].as<float>();
			cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
			cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
			cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
			cc2d.isSensor = circleCollider2DComponent["IsSensor"].as<bool>();
			if (circleCollider2DComponent["MaskBits"])
				cc2d.maskBits = circleCollider2DComponent["MaskBits"].as<UINT16>();
			if (circleCollider2DComponent["CategoryBits"])
				cc2d.categoryBits = circleCollider2DComponent["CategoryBits"].as<UINT16>();
		}

		auto revoluteJointComponent = entity["RevoluteJointComponent"];
		if (revoluteJointComponent)
		{
			auto& joint = deserializedEntity.AddComponent<RevoluteJointComponent>();
			joint.Opponent = revoluteJointComponent["Opponent"].as<uint64_t>();
			joint.LocalAnchor = revoluteJointComponent["LocalAnchor"].as<Vector2>();
			joint.OpponentLocalAnchor = revoluteJointComponent["OpponentLocalAnchor"].as<Vector2>();
			joint.EnableLimit = revoluteJointComponent["EnableLimit"].as<bool>();
			joint.AngleRange = revoluteJointComponent["AngleRange"].as<Vector2>();
		}

		auto distanceJointComponent = entity["DistanceJointComponent"];
		if (distanceJointComponent)
		{
			auto& joint = deserializedEntity.AddComponent<DistanceJointComponent>();
			joint.Opponent = distanceJointComponent["Opponent"].as<uint64_t>();
			joint.LocalAnchor = distanceJointComponent["LocalAnchor"].as<Vector2>();
			joint.OpponentLocalAnchor = distanceJointComponent["OpponentLocalAnchor"].as<Vector2>();
			joint.minLength = distanceJointComponent["minLength"].as<float>();
			joint.maxLength = distanceJointComponent["maxLength"].as<float>();
			joint.stiffness = distanceJointComponent["stiffness"].as<float>();
			joint.damping = distanceJointComponent["damping"].as<float>();
		}

		auto panelComponent = entity["PanelComponent"];
		if (panelComponent)
		{
			auto& panel = deserializedEntity.AddComponent<PanelComponent>();
			if (panelComponent["PanelPath"])
			{
				std::string texturePath = panelComponent["PanelPath"].as<std::string>();
				auto path = Project::GetAssetFileSystemPath(texturePath);
				panel.panel = make_shared<Texture>();
				panel.panel->Load(path.wstring(), false);
			}
			panel.resizeTexture = panelComponent["resizeTexture"].as<Vector2>();
			panel.offset = panelComponent["panelOffset"].as<Vector2>();
			panel.color = panelComponent["tineColor"].as<Vector4>();
			panel.tintOffset = panelComponent["tineRange"].as<Vector2>();
		}

		auto sliderComponent = entity["SliderComponent"];
		if (sliderComponent)
		{
			auto& slider = deserializedEntity.AddComponent<SliderComponent>();
			if (sliderComponent["barPath"])
			{
				std::string texturePath = sliderComponent["barPath"].as<std::string>();
				auto path = Project::GetAssetFileSystemPath(texturePath);
				slider.bar = make_shared<Texture>();
				slider.bar->Load(path.wstring(), false);
			}

			if (sliderComponent["gaugePath"])
			{
				std::string texturePath = sliderComponent["gaugePath"].as<std::string>();
				auto path = Project::GetAssetFileSystemPath(texturePath);
				slider.gauge = make_shared<Texture>();
				slider.gauge->Load(path.wstring(), false);
			}

			slider.showNumeric = sliderComponent["showNumeric"].as<bool>();
			slider.currentValue = sliderComponent["currentValue"].as<float>();
			slider.maxValue = sliderComponent["maxValue"].as<float>();
		}

		auto slotComponent = entity["SlotComponent"];
		if (slotComponent)
		{
			auto& slot = deserializedEntity.AddComponent<SlotComponent>();

			slot.rotation = slotComponent["Rotation"].as<float>();
			slot.itemSize = slotComponent["ItemSize"].as<Vector2>();
			if (slotComponent["slotPath"])
			{
				std::string texturePath = slotComponent["slotPath"].as<std::string>();
				auto path = Project::GetAssetFileSystemPath(texturePath);
				slot.slot = make_shared<Texture>();
				slot.slot->Load(path.wstring(), false);
			}
			if (slotComponent["maskPath"])
			{
				std::string texturePath = slotComponent["maskPath"].as<std::string>();
				auto path = Project::GetAssetFileSystemPath(texturePath);
				slot.slotMask = make_shared<Texture>();
				slot.slotMask->Load(path.wstring(), false);
			}
			if (slotComponent["itemPath"])
			{
				std::string texturePath = slotComponent["itemPath"].as<std::string>();
				auto path = Project::GetAssetFileSystemPath(texturePath);
				slot.item = make_shared<Texture>();
				slot.item->Load(path.wstring(), false);
			}
		}

		auto iconComponent = entity["IconComponent"];
		if (iconComponent)
		{
			auto& icon = deserializedEntity.AddComponent<IconComponent>();
			if (iconComponent["iconPath"])
			{
				std::string texturePath = iconComponent["iconPath"].as<std::string>();
				auto path = Project::GetAssetFileSystemPath(texturePath);
				icon.icon = make_shared<Texture>();
				icon.icon->Load(path.wstring(), false);
			}
			if (iconComponent["tint"])
			{
				icon.tint = iconComponent["tint"].as<Vector4>();
			}
			
		}

		auto particleSystem = entity["ParticleSystem"];
		if (particleSystem)
		{
			auto& particle = deserializedEntity.AddComponent<ParticleSystem>();
			
			particle.Position = particleSystem["Position"].as<Vector2>();
			particle.PositionVariation = particleSystem["PositionVariation"].as<Vector2>();

			particle.Velocity = particleSystem["Velocity"].as<Vector2>();
			particle.VelocityVariation = particleSystem["VelocityVariation"].as<Vector2>();
			particle.VelocityEnd = particleSystem["VelocityEnd"].as<Vector2>();

			particle.SizeBegin = particleSystem["SizeBegin"].as<Vector2>();
			particle.SizeEnd = particleSystem["SizeEnd"].as<Vector2>();
			particle.SizeVariation = particleSystem["SizeVariation"].as<Vector2>();

			particle.ColorBegin = particleSystem["ColorBegin"].as<Vector4>();
			particle.ColorEnd = particleSystem["ColorEnd"].as<Vector4>();

			particle.EmissionBegin = particleSystem["EmissionBegin"].as<Vector4>();
			particle.EmissionEnd = particleSystem["EmissionEnd"].as<Vector4>();

			particle.MaxParticles = particleSystem["MaxParticles"].as<UINT>();
			particle.Frequency = particleSystem["Frequency"].as<float>();
			particle.LifeTime = particleSystem["LifeTime"].as<float>();

			particle.PositionPolar = particleSystem["PositionPolar"].as<bool>();
			particle.VelocityPolar = particleSystem["VelocityPolar"].as<bool>();

			particle.state = (ParticleState)particleSystem["State"].as<UINT>();

			if (particleSystem["AttachTexture"]) {

				auto sPath = particleSystem["AttachTexture"].as<string>();
				auto path = Project::GetAssetFileSystemPath(sPath);
				particle.attachTexture = GET_SINGLE(Resources)->Load<Texture>(path.wstring(), path.wstring(), false);
				particle.TextureAttach = true;

				particle.AttachOffset = particleSystem["AttachOffset"].as<Vector2>();
			}
			if (particleSystem["GraphicsTexture"]) {
				auto sPath = particleSystem["GraphicsTexture"].as<string>();
				particle.graphicsTexture = GET_SINGLE(Resources)->Load<Texture>(stow(sPath), stow(sPath), false);
			}

			if (particleSystem["UseLocalCoord"])
				particle.UseLocalCoord = particleSystem["UseLocalCoord"].as<bool>();

			if (particleSystem["UseAliveZone"])
				particle.UseAliveZone = particleSystem["UseAliveZone"].as<bool>();

			if (particleSystem["AliveZone"])
				particle.aliveZone = particleSystem["AliveZone"].as<Vector2>();

			if (particleSystem["GraphicsShader"]) {
				auto sKey = particleSystem["GraphicsShader"].as<string>();
				particle.mMaterial->SetShader(GET_SINGLE(Resources)->Find<Shader>(stow(sKey)));
			}
		}

		auto bloom = entity["Bloom"];
		if (bloom)
		{
			auto& bloomComp = deserializedEntity.AddComponent<Bloom>();

			bloomComp.Threshold = bloom["Threshold"].as<float>();
			bloomComp.Intensity = bloom["Intensity"].as<float>();
		}

		auto trail = entity["TrailRenderer"];
		if (trail)
		{
			auto& trailComp = deserializedEntity.AddComponent<TrailRenderer>();

			trailComp.Color = trail["Color"].as<Vector4>();
			trailComp.base = trail["Base"].as<Vector3>();
			trailComp.tip = trail["Tip"].as<Vector3>();

			auto shaderKey = trail["Shader"].as<string>();
			auto texturePath = trail["Texture"].as<string>();
			auto path = Project::GetAssetFileSystemPath(texturePath);

			trailComp.material->SetShader(GET_SINGLE(Resources)->Find<Shader>(stow(shaderKey)));
			trailComp.material->SetTexture(0, GET_SINGLE(Resources)->Load<Texture>(path.wstring(), path.wstring(), false));
		}

		auto line = entity["LineRenderer"];
		if (line)
		{
			auto& lineComp = deserializedEntity.AddComponent<LineRenderer>();

			lineComp.Color = line["Color"].as<Vector4>();
			lineComp.Emission = line["Emission"].as<Vector4>();

			lineComp.Size = line["Size"].as<Vector2>();
			lineComp.Velocity = line["Velocity"].as<Vector2>();
		}

		return deserializedEntity;
	}
}