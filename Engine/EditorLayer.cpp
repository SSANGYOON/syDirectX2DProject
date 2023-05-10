#include "pch.h"
#include "EditorLayer.h"
#include "SceneSerializer.h"
#include "SceneManager.h"
#include "Resources.h"
#include "FileDialogs.h"
#include "ImGuizmo.h"
#include "Resources.h"
#include "PrefabManager.h"
#include "Application.h"
#include "Project.h"
#include "Renderer.h"
#include "RenderTargetGroup.h"
#include "MouseEvent.h"
#include "imGuiLayer.h"
#include "Engine.h"
#include "ScriptEngine.h"
namespace SY {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_SquareColor(Vector4(0.2f, 0.3f, 0.8f, 1.0f))
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{

		m_CheckerboardTexture = GET_SINGLE(Resources)->Load<Texture>(L"CheckerBoard", L"assets/textures/Checkerboard.png");
		m_IconPlay = GET_SINGLE(Resources)->Load<Texture>(L"PlayButton", L"Icons/PlayButton.png");
		m_IconPause = GET_SINGLE(Resources)->Load<Texture>(L"PauseButton", L"Icons/PauseButton.png");
		m_IconSimulate = GET_SINGLE(Resources)->Load<Texture>(L"SimulateButton", L"Icons/SimulateButton.png");
		m_IconStep = GET_SINGLE(Resources)->Load<Texture>(L"StepButton", L"Icons/StepButton.png");
		m_IconStop = GET_SINGLE(Resources)->Load<Texture>(L"StopButton", L"Icons/StopButton.png");

		m_EntityTex = make_shared<Texture>();

		m_ViewportSize = Vector2(1920, 1080);
		m_EntityTex->Create(m_ViewportSize.x, m_ViewportSize.y, DXGI_FORMAT_R32_SINT, 0, D3D11_CPU_ACCESS_READ);
		m_EditorScene = shared_ptr<Scene>();
		m_ActiveScene = m_EditorScene;

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);
		}
		else
		{
			if (!OpenProject())
				Application::Get().Close();
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 1.f, 1000.0f);


	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(float timeStep)
	{
		m_EditorCamera.SetWheelStep(CameraWheelStep);
		m_EditorCamera.SetMoveSpeed(CameraSpeed);

		//m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		GEngine->BindRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::EDITOR);
		GEngine->ClearRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::EDITOR);

		switch (m_SceneState)
		{
		case SceneState::Edit:
		{
			m_EditorCamera.OnUpdate(timeStep);

			m_ActiveScene->OnUpdateEditor(timeStep, m_EditorCamera);
			break;
		}
		case SceneState::Simulate:
		{
			m_EditorCamera.OnUpdate(timeStep);

			m_ActiveScene->OnUpdateSimulation(timeStep, m_EditorCamera);
			break;
		}
		case SceneState::Play:
		{
			SceneManager::OnUpdate(timeStep);

			if (m_ActiveScene != SceneManager::GetActiveScene()) {
				m_ActiveScene = SceneManager::GetActiveScene();
				m_SceneHierarchyPanel.SetContext(m_ActiveScene);
			}
			break;
		}
		}

		auto [mx, my] = ImGui::GetMousePos();
		int mouseX = (int)mx;
		int mouseY = (int)my;
		Vector2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

		if (mouseX >= m_ViewportBounds[0].x && mouseY >= m_ViewportBounds[0].y && mouseX < m_ViewportBounds[1].x && mouseY < m_ViewportBounds[1].y)
		{
			D3D11_BOX srcBox = {};

			Vector2 pixelPos = (Vector2(mx, my) - m_ViewportBounds[0]) / (m_ViewportBounds[1] - m_ViewportBounds[0]) * Vector2(1920.f, 1080.f);
			srcBox.left = (UINT)pixelPos.x;
			srcBox.right = (UINT)pixelPos.x + 1;
			srcBox.bottom = (UINT)pixelPos.y + 1;
			srcBox.top = (UINT)pixelPos.y;
			srcBox.front = 0;
			srcBox.back = 1;

			D3D11_MAPPED_SUBRESOURCE sub = {};

			CONTEXT->CopySubresourceRegion(m_EntityTex->GetD3Texture(), 0,0,0,0, GEngine->GetRenderTargetTex(RENDER_TARGET_GROUP_TYPE::EDITOR, 1)->GetD3Texture(),0, &srcBox);


			CONTEXT->Map(m_EntityTex->GetD3Texture(), 0, D3D11_MAP_READ, 0, &sub);

			int pixelData = *reinterpret_cast<int*>(sub.pData);

			CONTEXT->Unmap(m_EntityTex->GetD3Texture(), 0);

			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		OnOverlayRender();
	}

	void EditorLayer::OnImGuiRender()
	{

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
					OpenProject();

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open Scene"))
				{
					SaveScene();
					OpenScene();
				}

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
					SaveSceneAs();
				if (ImGui::MenuItem("Set current scene as entry"))
					SetCurrentSceneAsEntry();
				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::MenuItem("Show Imgui Panels", "Ctrl+P", &m_ShowImguiPanels);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		if (m_ShowImguiPanels) {
			m_SceneHierarchyPanel.OnImGuiRender();
			m_ContentBrowserPanel->OnImGuiRender();

			ImGui::Begin("Stats");

			auto& stats = Renderer::GetStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Used Instanced : %d", stats.Instanced);

			ImGui::End();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
			ImGui::DragFloat("CameraSpeed", &CameraSpeed, 0.1f, 0.1f, 3.0f);
			ImGui::DragFloat("MouseWheelStep", &CameraWheelStep, 0.1f, 0.1f, 3.0f);

			ImGui::DragInt("CollistionLayers", &CollisionLayers, 1, 1, 16);

			ImGui::End();
		}
		Renderer::ResetStats();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		ImGui::Image(reinterpret_cast<void*>(GEngine->GetRenderTargetTex(RENDER_TARGET_GROUP_TYPE::EDITOR,0)->GetSRV()), ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2(0,0), ImVec2(1, 1));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(path);
			}

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Prefab"))
			{
				uint64_t id = *(uint64_t*)payload->Data;
				PrefabManager::Instantiate(m_EditorScene.get(), id);
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);		

			const Matrix& cameraProjection = m_EditorCamera.GetProjection();
			Matrix cameraView = m_EditorCamera.GetView();

			// Entity transform
			if (selectedEntity.HasComponent<TransformComponent>()) {
				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				Matrix transform = tc.localToWorld;

				// Snapping
				bool snap = INPUT->GetKeyState(KEY_TYPE::LBUTTON) == KEY_STATE::PRESS;
				float snapValue = 0.5f; // Snap to 0.5m for translation/scale
				// Snap to 45 degrees for rotation
				if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(reinterpret_cast<float*>(&cameraView), reinterpret_cast<const float*>(&cameraProjection),
					(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::WORLD, reinterpret_cast<float*>(&transform),
					nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing())
				{
					Vector3 translation, rotation, scale;
					Quaternion quat;
					if (selectedEntity.HasComponent<Parent>())
					{
						transform = transform * m_EditorScene->GetEntityByUUID(selectedEntity.GetComponent<Parent>().parentHandle).GetComponent<TransformComponent>().localToWorld.Invert();
					}
					assert(transform.Decompose(scale, quat, translation));
					rotation = quat.ToEuler();
					Vector3 deltaRotation = rotation - tc.rotation;
					tc.translation = translation;
					tc.rotation += deltaRotation;
					tc.scale = scale;
				}
			}
		}


		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_ActiveScene;

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
			tintColor.w = 0.5f;

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		bool hasPlayButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play;
		bool hasSimulateButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate;
		bool hasPauseButton = m_SceneState != SceneState::Edit;

		if (hasPlayButton)
		{
			shared_ptr<Texture> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetSRV(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
		}

		if (hasSimulateButton)
		{
			if (hasPlayButton)
				ImGui::SameLine();

			shared_ptr<Texture> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetSRV(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}
		if (hasPauseButton)
		{
			bool isPaused = m_ActiveScene->IsPaused();
			ImGui::SameLine();
			{
				shared_ptr<Texture> icon = m_IconPause;
				if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetSRV(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
				{
					m_ActiveScene->SetPaused(!isPaused);
				}
			}

			// Step button
			if (isPaused)
			{
				ImGui::SameLine();
				{
					shared_ptr<Texture> icon = m_IconStep;
					bool isPaused = m_ActiveScene->IsPaused();
					if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetSRV(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
					{
						m_ActiveScene->Step();
					}
				}
			}
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (m_SceneState == SceneState::Edit)
			m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		bool control = INPUT->GetKeyState(KEY_TYPE::LCTRL) == KEY_STATE::PRESS || INPUT->GetKeyState(KEY_TYPE::RCTRL) == KEY_STATE::PRESS;
		bool shift = INPUT->GetKeyState(KEY_TYPE::LSHIFT) == KEY_STATE::PRESS || INPUT->GetKeyState(KEY_TYPE::RSHIFT) == KEY_STATE::PRESS;

		auto keyc = INPUT->GetKeyState(KEY_TYPE::LCTRL);
		auto keys = INPUT->GetKeyState(KEY_TYPE::LSHIFT);
		switch (e.GetKeyCode())
		{
		case KEY_TYPE::N:
		{
			if (control)
				NewScene();

			break;
		}
		case KEY_TYPE::O:
		{
			if (control)
				OpenProject();

			break;
		}
		case KEY_TYPE::S:
		{
			if (control)
			{
				if (shift)
					SaveSceneAs();
				else
					SaveScene();
			}
			break;
		}

		case KEY_TYPE::P:
		{
			if (control)
			{
				m_ShowImguiPanels = !m_ShowImguiPanels;
			}
			break;
		}

		case KEY_TYPE::D:
		{
			if (control)
				OnDuplicateEntity();

			break;
		}

		// Gizmos
		case KEY_TYPE::Q:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = -1;
			break;
		}
		case KEY_TYPE::W:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case KEY_TYPE::E:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		case KEY_TYPE::R:
		{

			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::SCALE;

			break;
		}
		case KEY_TYPE::DELETEKEY:
		{
			if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
			{
				Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
				if (selectedEntity)
				{
					m_SceneHierarchyPanel.SetSelectedEntity({});
					m_ActiveScene->DestroyEntity(selectedEntity);
				}
			}
			break;
		}
		case KEY_TYPE::LBUTTON:
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && INPUT->GetKeyState(KEY_TYPE::LALT) != KEY_STATE::PRESS)
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			break;
		}
		}
		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (!camera)
				return;

			Renderer::Begin(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().localToWorld);
		}
		else
		{
			Renderer::Begin(m_EditorCamera);
		}

		if (m_ShowPhysicsColliders)
		{
			// Box Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					Matrix transform = Matrix::CreateScale(Vector3(bc2d.Size * 2.f,1.f)) * Matrix::CreateTranslation(Vector3(bc2d.Offset, 0.001f)) * tc.localToWorld;


					Renderer::DrawRectOutline(transform, Vector4(0, 1, 0, 1), entity);
				}
			}

			// Circle Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					Matrix transform = Matrix::CreateScale(Vector3(cc2d.Radius * 2.f)) * Matrix::CreateTranslation(Vector3(cc2d.Offset, 0.001f)) * tc.localToWorld;

					Renderer::DrawCircleOutline(transform, Vector4(0, 1, 0, 1), entity);
				}
			}
		}

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();


		Renderer::End();
	}

	void EditorLayer::NewProject()
	{
		Project::New();
	}

	void EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		if (Project::Load(path))
		{
			ScriptEngine::Init();
			PrefabManager::Init();
			auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
			OpenScene(startScenePath);
			m_ContentBrowserPanel = make_unique<ContentBrowserPanel>();

		}
	}

	bool EditorLayer::OpenProject()
	{
		std::string filepath = FileDialogs::OpenFile("Hazel Project (*.hproj)\0*.hproj\0");
		if (filepath.empty())
			return false;

		OpenProject(filepath);
		return true;
	}

	void EditorLayer::SaveProject()
	{
		//Project::SaveActive();
	}

	void EditorLayer::NewScene()
	{
		m_EditorScene = make_shared<Scene>();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		if (path.extension().string() != ".hazel")
		{
			assert(false);
			return;
		}

		shared_ptr<Scene> newScene = make_shared<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			std::filesystem::path stdpath(filepath);


			m_EditorScenePath = "Scenes/" + stdpath.filename().string();
		}
	}

	void EditorLayer::SetCurrentSceneAsEntry()
	{
		Project::GetActive()->GetConfig().StartScene = m_EditorScenePath;
		std::string filepath = FileDialogs::SaveFile("Hazel Project (*.hproj)\0*.proj\0");
		Project::SaveActive(filepath);
	}

	void EditorLayer::SerializeScene(shared_ptr<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		
		SceneManager::SetStartScene(m_ActiveScene, "start");

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		assert(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

		if (m_SceneState == SceneState::Play)
			SceneManager::OnStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnScenePause()
	{
		if (m_SceneState == SceneState::Edit)
			return;

		SceneManager::GetActiveScene()->SetPaused(true);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
		{
			Entity newEntity = m_EditorScene->DuplicateEntity(selectedEntity);
			m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
		}
	}
}