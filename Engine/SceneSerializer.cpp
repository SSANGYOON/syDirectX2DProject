#include "pch.h"
#include "Resources.h"
#include "SceneSerializer.h"
#include "Scene.h"
#include "Entity.h"
#include "ScriptEngine.h"

#include "Project.h"

#include "SerializerUtils.h"
#include "Animation.h"
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

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
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

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.scale;

			out << YAML::EndMap; // TransformComponent
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

			out << YAML::EndMap; // CameraComponent
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
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
			if (spriteRendererComponent.Texture) {
				wstring wpath = spriteRendererComponent.Texture->GetPath();

				out << YAML::Key << "TexturePath" << YAML::Value << wtos(wpath);
				out << YAML::Key << "SourceOffset" << YAML::Value << spriteRendererComponent.spCB.sourceOffset;
				out << YAML::Key << "SourceSize" << YAML::Value << spriteRendererComponent.spCB.sourceSize;
				out << YAML::Key << "SourceSheetSize" << YAML::Value << spriteRendererComponent.spCB.sourceSheetSize;
			}

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;

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

			out << YAML::EndMap; // CircleCollider2DComponent
		}

		if (entity.HasComponent<SpriteAnimatorComponent>())
		{
			out << YAML::Key << "AnimatorComponent";
			out << YAML::BeginMap; // AnimatorComponent

			auto& animator = entity.GetComponent<SpriteAnimatorComponent>();
			if(animator.clips.size())
				out << YAML::Key << "ClipPath" << YAML::Value << wtos(animator.clips.begin()->second->GetPath());


			out << YAML::EndMap; // CircleCollider2DComponent
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
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				//HZ_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.translation = transformComponent["Translation"].as<Vector3>();
					tc.rotation = transformComponent["Rotation"].as<Vector3>();
					tc.scale = transformComponent["Scale"].as<Vector3>();
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
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<Vector4>();
					if (spriteRendererComponent["TexturePath"])
					{
						std::string texturePath = spriteRendererComponent["TexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						src.Texture = make_shared<Texture>();
						src.Texture->Load(path.wstring(), false);

						src.spCB.sourceOffset = spriteRendererComponent["SourceOffset"].as<Vector2>();
						src.spCB.sourceSize = spriteRendererComponent["SourceSize"].as<Vector2>();
						src.spCB.sourceSheetSize = spriteRendererComponent["SourceSheetSize"].as<Vector2>();
					}
				}

				auto animatorComponent = entity["AnimatorComponent"];
				if (animatorComponent)
				{
					auto& animator = deserializedEntity.AddComponent<SpriteAnimatorComponent>();

					animator._currentTime = 0.f;
					animator._lastTime = -1.f;
					animator._endedAt = 0.f;
					animator._transitionTime = 0.f;
					animator._transitionElapsed = 0.f;
					animator._prevClip = nullptr;
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

						if (!data["Animations"])
							return S_FALSE;
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

							string key;
							key = Key.as<string>();
							shared_ptr<Animation> clipRef;
							if (clipRef = GET_SINGLE(Resources)->Find<Animation>(stow(key)))
							{
							}
							else {
								Vector2 offset = OriginalOffset.as<Vector2>();
								Vector2 size = SpriteSize.as<Vector2>();
								Vector2 step = Step.as<Vector2>();
								UINT columns = Columns.as<UINT>();
								UINT frame = Frames.as<UINT>();
								float duration = Duration.as<float>();

								Vector2 targetOffset = Vector2::Zero;
								if (clip["TargetOffset"])
									targetOffset = clip["TargetOffset"].as<Vector2>();
								clipRef = make_shared<Animation>();
								clipRef->Load(offset, size, step, targetOffset, columns, frame, duration, key);
								clipRef->SetPath(stow(relativePath));
								GET_SINGLE(Resources)->Insert(stow(key), clipRef);
							}



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
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		// Not implemented
		assert(false);
		return false;
	}

}