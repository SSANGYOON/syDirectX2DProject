#include "pch.h"
#include "SceneHierarchyPanel.h"
#include "ParentManager.h"
#include "PrefabManager.h"
#include "Component.h"
#include "ScriptEngine.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "UI.h"

#include "Texture.h"
#include <cstring>
#include "Animation.h"
#include "Resources.h"
#include "SerializerUtils.h"
#include "SceneSerializer.h"
#include "Scene.h"
#include "FileDialogs.h"
#include <queue>
namespace SY {

	SceneHierarchyPanel::SceneHierarchyPanel(const shared_ptr<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const shared_ptr<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID , m_Context.get() };
					if(entity.IsValid())
						if (!entity.HasComponent<Parent>())
							DrawEntityNode(entity);
				});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			// Right-click on blank space
			if (!m_SelectionContext && ImGui::BeginPopupContextWindow(0, 1))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					m_Context->CreateEntity("Empty Entity");

				ImGui::EndPopup();
			}

		}


		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
		{
			m_SelectionContext = entity;
		}



		bool entityDeleted = false;
		if (m_SelectionContext == entity && ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			if (entity.HasComponent<Parent>())
				if (ImGui::MenuItem("Set Independent"))
					entity.RemoveComponent<Parent>();
			if (ImGui::MenuItem("Save as prefab")){
				string filepath = FileDialogs::SaveFile("SY Prefab (*.pref)\0*.pref\0");

				YAML::Emitter out;
				out << YAML::BeginMap;
				out << YAML::Key << "Head" << YAML::Value << entity.GetUUID();
				out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
				bool hasparent = entity.HasComponent<Parent>();
				UUID parentID;
				if (hasparent) {
					parentID = entity.GetComponent<Parent>().parentHandle;
					entity.RemoveComponent<Parent>();
				}

				Scene* context = entity.GetContext();
				queue<UINT> childQueue;

				childQueue.push((UINT)entity);

				while (!childQueue.empty())
				{
					Entity ent = { (entt::entity)childQueue.front(), context };
					childQueue.pop();

					SceneSerializer::SerializeEntity(out, ent);

					for (auto child : ParentManager::GetChildren(ent))
					{
						childQueue.push(child);	
					}
				}
				
				out << YAML::EndSeq;
				out << YAML::EndMap;

				std::ofstream fout(filepath);
				fout << out.c_str();

				if (hasparent)
				{
					auto& parent = entity.AddComponent<Parent>();
					parent.parentHandle = parentID;
				}
			}

			ImGui::EndPopup();
		}


		if (ImGui::BeginDragDropSource())
		{
			UUID uuid = entity.GetUUID();
			ImGui::SetDragDropPayload("Entity data", &uuid, sizeof(UUID));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity data"))
			{
				UUID uuid = *(UUID*)payload->Data;
				Entity child = m_Context->GetEntityByUUID(uuid);

				child.AddOrReplaceComponent<Parent>(entity.GetUUID());
			}
			ImGui::EndDragDropTarget();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			
			auto& children = ParentManager::GetChildren((UINT)entity);

			for (const UINT child : children)
				DrawEntityNode({ (entt::entity)child, m_Context.get() });

			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			queue<UINT> childQueue;

			childQueue.push((UINT)entity);
			while (!childQueue.empty())
			{
				Entity ent = { (entt::entity)childQueue.front(), m_Context.get()};
				childQueue.pop();

				for (auto child : ParentManager::GetChildren(ent))
				{
					childQueue.push(child);
				}

				m_Context->DestroyEntity(ent);
				if (m_SelectionContext == ent)
					m_SelectionContext = {};
			}			
		}
	}

	void DrawVec3Control(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(
			);
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}

			
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		const char* states[] = { "Active", "Pause", "Dead" };

		auto& state = entity.GetComponent<StateComponent>().state;
		if (ImGui::BeginCombo("State", states[(int)state]))
		{
			for (int i = 0; i < 3; i++)
			{
				bool is_selected = (int)state == i;
				if(ImGui::Selectable(states[i], is_selected))
					state = (EntityState)i;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<TransformComponent>("Transform");
			DisplayAddComponentEntry<RectTransformComponent>("RectTransform");
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
			DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");

			DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
			DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
			DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");
			DisplayAddComponentEntry<RevoluteJointComponent>("RevoluteJoint");
			DisplayAddComponentEntry<DistanceJointComponent>("DistanceJoint");
			DisplayAddComponentEntry<ScriptComponent>("C# Script");
			DisplayAddComponentEntry<SpriteAnimatorComponent>("SpriteAnimator");
			DisplayAddComponentEntry<TransformAnimatorComponent>("TransformAnimator");
			DisplayAddComponentEntry<PanelComponent>("Panel");
			DisplayAddComponentEntry<SliderComponent>("Slider");
			DisplayAddComponentEntry<SlotComponent>("Slot");
			DisplayAddComponentEntry<IconComponent>("Icon");
			DisplayAddComponentEntry<Eraser>("Eraser");
			DisplayAddComponentEntry<ParticleSystem>("ParticleSystem");
			DisplayAddComponentEntry<BackGroundColorComponent>("BackGroundColor");
			DisplayAddComponentEntry<Light>("Light");
			DisplayAddComponentEntry<Bloom>("Bloom");
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, &TransformComponent::DrawImGui);

		DrawComponent<RectTransformComponent>("RectTransform", entity, &RectTransformComponent::DrawImGui);

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveVerticalFov = camera.GetPerspectiveVerticalFOV() * 180.f / DirectX::XM_PI;
					if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
						camera.SetPerspectiveVerticalFOV(perspectiveVerticalFov / 180.f * DirectX::XM_PI);

					float perspectiveNear = camera.GetNearClip();
					if (ImGui::DragFloat("Near", &perspectiveNear))
						camera.SetNearClip(perspectiveNear);

					float perspectiveFar = camera.GetFarClip();
					if (ImGui::DragFloat("Far", &perspectiveFar))
						camera.SetFarClip(perspectiveFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.GetNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetNearClip(orthoNear);

					float orthoFar = camera.GetFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetFarClip(orthoFar);

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				}

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

				ImGui::InputFloat("OscillationAmp", &component.oscillationAmp);
				ImGui::InputFloat("OscillationFreq", &component.oscillationFreq);
				ImGui::InputFloat("OscillationDamp", &component.oscillationDamp);
			});

		DrawComponent<ScriptComponent>("C# Script", entity, [entity, scene = m_Context](auto& component) mutable
			{
				bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

				static char buffer[64];
				strcpy_s(buffer, sizeof(buffer), component.ClassName.c_str());

				UI::ScopedStyleColor textColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f), !scriptClassExists);

				if (ImGui::InputText("Class", buffer, sizeof(buffer)))
				{
					component.ClassName = buffer;
					return;
				}

				bool sceneRunning = scene->IsRunning();
				if (sceneRunning)
				{
					shared_ptr<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
					if (scriptInstance)
					{
						const auto& fields = scriptInstance->GetScriptClass()->GetFields();
						for (const auto& [name, field] : fields)
						{
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptInstance->GetFieldValue<float>(name);
								if (ImGui::DragFloat(name.c_str(), &data))
								{
									scriptInstance->SetFieldValue(name, data);
								}
							}

							

							if (field.Type == ScriptFieldType::Vector2)
							{
								Vector2 data = scriptInstance->GetFieldValue<Vector2>(name);
								if (ImGui::DragFloat2(name.c_str(), reinterpret_cast<float*>(&data)))
								{
									scriptInstance->SetFieldValue(name, data);
								}
							}
						}
					}
				}
				else
				{
					if (scriptClassExists)
					{
						shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
						const auto& fields = entityClass->GetFields();

						auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
						for (const auto& [name, field] : fields)
						{
							// Field has been set in editor
							if (entityFields.find(name) != entityFields.end())
							{
								ScriptFieldInstance& scriptField = entityFields.at(name);

								// Display control to set it maybe
								if (field.Type == ScriptFieldType::Float)
								{
									float data = scriptField.GetValue<float>();
									if (ImGui::DragFloat(name.c_str(), &data))
										scriptField.SetValue(data);
								}

								if (field.Type == ScriptFieldType::Vector2)
								{
									Vector2 data = scriptField.GetValue<Vector2>();
									if (ImGui::DragFloat2(name.c_str(), reinterpret_cast<float*>(&data)))
										scriptField.SetValue(data);
								}

								if (field.Type == ScriptFieldType::ULong)
								{
									UINT64 data = scriptField.GetValue<UINT64>();
									if (ImGui::InputScalar(name.c_str(), ImGuiDataType_U64, &data))
										scriptField.SetValue(data);
								}

								if (field.Type == ScriptFieldType::Entity)
								{
									UUID data = scriptField.GetValue<UUID>();

									if (data)
									{
										ImGui::Text(PrefabManager::GetPrefabName(data).c_str());
									}

									ImGui::Text((name + " From Prefab").c_str());

									if (ImGui::BeginDragDropTarget()) {
										if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Prefab")) {
											uint64_t id = *reinterpret_cast<uint64_t*>(payload->Data);
											scriptField.SetValue(id);
										}
									}
								}

								if (field.Type == ScriptFieldType::String)
								{
									char* data = scriptField.GetString();
									if (ImGui::InputText("String", data, 256))
										scriptField.SetString(data);
								}
							}
							else
							{
								// Display control to set it maybe
								if (field.Type == ScriptFieldType::Float)
								{
									float data = 0.0f;
									if (ImGui::DragFloat(name.c_str(), &data))
									{
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue(data);
									}
								}

								if (field.Type == ScriptFieldType::Vector2)
								{
									Vector2 data = Vector2::Zero;
									if (ImGui::DragFloat2(name.c_str(), reinterpret_cast<float*>(&data)))
									{
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue(data);
									}
								}

								if (field.Type == ScriptFieldType::ULong)
								{
									UINT64 data = 0;
									if (ImGui::InputScalar(name.c_str(), ImGuiDataType_U64, &data)) {
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue(data);
									}
								}

								if (field.Type == ScriptFieldType::Entity)
								{
									ImGui::Text((name + " From Prefab").c_str());
									if (ImGui::BeginDragDropTarget()) {
										if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Prefab")) {
											uint64_t id = *reinterpret_cast<uint64_t*>(payload->Data);
											ScriptFieldInstance& fieldInstance = entityFields[name];
											fieldInstance.Field = field;
											fieldInstance.SetValue(id);
										}
									}
								}

								if (field.Type == ScriptFieldType::String)
								{
									char empty[256] = "";
									if (ImGui::InputText(name.c_str(), empty,256)) {
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetString(const_cast<char*>(empty));
									}
								}
							}
						}
					}
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, &SpriteRendererComponent::DrawImGui);
		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, &CircleRendererComponent::DrawImGui);
		DrawComponent<Eraser>("Eraser", entity, &Eraser::DrawImGui);
		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, &Rigidbody2DComponent::DrawImGui);
		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, &BoxCollider2DComponent::DrawImGui);
		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, &CircleCollider2DComponent::DrawImGui);
		DrawComponent<RevoluteJointComponent>("RevoluteJoint", entity, &RevoluteJointComponent::DrawImGui);
		DrawComponent<DistanceJointComponent>("DistanceJoint", entity, &DistanceJointComponent::DrawImGui);
		DrawComponent<Light>("Light", entity, &Light::DrawImGui);
		DrawComponent<TransformAnimatorComponent>("TransformAnimator", entity, [] (auto& component){
			
			vector<string> animations;
			for (auto anim : component.clips)
				animations.push_back(anim.first);

			for (string& key : animations)
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
				if (ImGui::TreeNodeEx(key.c_str(), flags))
				{
					shared_ptr<TransformAnimation> clip = component.clips[key];

					if (ImGui::BeginPopupContextWindow(key.c_str()))
					{
						if (ImGui::MenuItem("Delete clip"))
						{
							component.clips.erase(component.clips.find(key));
						}
						ImGui::EndPopup();
					}
					ImGui::TreePop();
				}
			}

			ImGui::Text("Drag new clips here", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path animPath(path);

					YAML::Node data;
					wstring fullPath = path;

					try
					{
						data = YAML::LoadFile(wtos(path));
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
						auto Key = clip["ClipName"];
						auto Frames = clip["Frames"];
						auto Duration = clip["Duration"];
						auto Loop = clip["Loop"];

						string key;
						key = Key.as<string>();
						shared_ptr<TransformAnimation> clipRef = GET_SINGLE(Resources)->Find<TransformAnimation>(stow(key));
						if (clipRef == nullptr){
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
						component.clips[key] = clipRef;
					}
				}
				ImGui::EndDragDropTarget();
			}
			});

		DrawComponent<SpriteAnimatorComponent>("SpriteAnimator", entity, [this](auto& component)
			{
				vector<string> animations;
				for (auto anim : component.clips)
					animations.push_back(anim.first);

				for (string& key : animations)
				{
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
					if(ImGui::TreeNodeEx(key.c_str(), flags))
					{
						shared_ptr<Animation> clip = component.clips[key];

						SpriteRendererComponent& sr = this->GetSelectedEntity().GetComponent<SpriteRendererComponent>();
						sr.Offset = clip->GetOffset();
						sr.tile = clip->GetSize();

						if (component._startEvent.find(key) != component._startEvent.end())
						{
							char buffer[64];
							strcpy_s(buffer,64, component._startEvent[key].c_str());
							ImGui::InputText("StartEvent",buffer, sizeof(buffer));
							bool erased;
							if(erased = ImGui::Button("Remove Start Event"))
								component._startEvent.erase(key);
							if (!erased)
								component._startEvent[key] = string(buffer);
						}
						else
						{
							if(ImGui::Button("Set Start Event"))
								component._startEvent[key] = "";
						}

						if (component._endEvent.find(key) != component._endEvent.end())
						{
							char buffer[64];
							strcpy_s(buffer, 64, component._endEvent[key].c_str());
							ImGui::InputText("EndEvent", buffer, sizeof(buffer));
							bool erased;
							if (erased = ImGui::Button("Remove End Event"))
								component._endEvent.erase(key);
							if (!erased)
								component._endEvent[key] = string(buffer);
						}
						else
						{
							if (ImGui::Button("Set End Event"))
								component._endEvent[key] = "";
						}

						if (ImGui::BeginPopupContextWindow(key.c_str()))
						{
							if (ImGui::MenuItem("Delete clip"))
							{
								component.clips.erase(component.clips.find(key));
							}
							ImGui::EndPopup();
						}
						ImGui::TreePop();
					}		
				}

				ImGui::Text("Drag new clips here", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path animPath(path);
						
						YAML::Node data;
						wstring fullPath = path;
						
						try
						{
							data = YAML::LoadFile(wtos(path));
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
							auto Loop = clip["Loop"];
							auto nextKey = clip["nextKey"];

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
								bool loop = Loop.as<bool>();
								string nextClipKey = "";
								if (nextKey)
									nextClipKey = nextKey.as<string>();
								Vector2 targetOffset = Vector2::Zero;
								if (clip["TargetOffset"])
									targetOffset = clip["TargetOffset"].as<Vector2>();
								clipRef = make_shared<Animation>();
								clipRef->Load(offset, size, step, targetOffset, columns, frame, duration, key,loop, nextClipKey);
								GET_SINGLE(Resources)->Insert(stow(key), clipRef);
							}

							std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
							auto resourcedir = parentPath / "Resources";
							clipRef->SetPath(filesystem::relative(fullPath, resourcedir).wstring());

							component.clips[key] = clipRef;
						}
					}
					ImGui::EndDragDropTarget();
				}
			});

			DrawComponent<PanelComponent>("Panel", entity, &PanelComponent::DrawImGui);
			DrawComponent<SliderComponent>("Slider", entity, &SliderComponent::DrawImGui);
			DrawComponent<SlotComponent>("Slot", entity, &SlotComponent::DrawImGui);
			DrawComponent<IconComponent>("Icon", entity, &IconComponent::DrawImGui);
			DrawComponent<ParticleSystem>("ParticleSystem", entity, &ParticleSystem::DrawImGui);
			DrawComponent<Bloom>("Bloom", entity, &Bloom::DrawImGui);

			DrawComponent<BackGroundColorComponent>("BackGroundColor", entity, [](auto& component)
				{
					ImGui::ColorEdit4("BackGroundColor", reinterpret_cast<float*>(&component.color));
				}
			);
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

}