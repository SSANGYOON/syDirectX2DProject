#include "pch.h"
#include "SceneHierarchyPanel.h"
#include "ParentManager.h"
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

					TransformComponent tr = m_Context->m_Registry.get<TransformComponent>(entityID);
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

		if (ImGui::IsItemClicked())
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
				out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
				bool hasparent = entity.HasComponent<Parent>();
				UUID parentID;
				if (hasparent) {
					parentID = entity.GetComponent<Parent>().parentHandle;
					entity.RemoveComponent<Parent>();
				}
				SerializeEntity(out, entity);
				Scene* context = entity.GetContext();
				for (auto child : ParentManager::GetChildren(entity))
				{
					Entity childEntity = { (entt::entity)child, context };
					SerializeEntity(out, childEntity);
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
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static void DrawVec3Control(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
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
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
			DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
			DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
			DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");
			DisplayAddComponentEntry<ScriptComponent>("C# Script");
			DisplayAddComponentEntry<SpriteAnimatorComponent>("SpriteAnimator");
			DisplayAddComponentEntry<TransformAnimatorComponent>("TransformAnimator");
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.translation);
				Vector3 rotation = component.rotation * 180.f / XM_PI;
				DrawVec3Control("Rotation", rotation);
				component.rotation = rotation / 180.f * XM_PI;
				DrawVec3Control("Scale", component.scale, 1.0f);
			});

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

				// Fields
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
							}
						}
					}
				}
			});


		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&component.Color));

				ImGui::Text("Texture", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath(path);
						shared_ptr<Texture> texture = make_shared<Texture>();
						texture->Load(texturePath.wstring(),false);
						assert(texture->GetD3Texture());
						component.Texture = texture;
						component.spCB.sourceSheetSize = texture->GetSize();
					}
					ImGui::EndDragDropTarget();
				}
				


				if (component.Texture) {
					ImGui::Text(wtos(component.Texture->GetPath()).c_str());

					if(ImGui::Button("Remove"))
					{
						component.Texture = nullptr;
					}
				}

				ImGui::InputFloat2("SourceOffset", reinterpret_cast<float*>(&component.spCB.sourceOffset));
				ImGui::InputFloat2("SourceSize", reinterpret_cast<float*>(&component.spCB.sourceSize));
				ImGui::InputFloat2("SourceSheetSize", reinterpret_cast<float*>(&component.spCB.sourceSheetSize));

				bool layerBits[8];
				int category = component.LayerBit;
				for (int i = 0; i < 8; i++)
				{
					layerBits[i] = category % 2;
					category /= 2;
				}

				ImGui::Text("Layer");
				for (int i = 0; i < 8; i++) {
					ImGui::Checkbox(( to_string(i)).c_str(), &layerBits[i]);
					if (i < 7)
						ImGui::SameLine();
				}

				component.LayerBit = 0;
				for (int i = 0; i < 8; i++)
				{
					component.LayerBit += layerBits[i] * (1 << i);
				}
			});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
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
			});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
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
			});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component) 
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
			});

		DrawComponent<TransformAnimatorComponent>("TransformAnimator", entity, [] (auto& component){});

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
						sr.spCB.sourceOffset = clip->GetOffset();
						sr.spCB.sourceSize = clip->GetSize();

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