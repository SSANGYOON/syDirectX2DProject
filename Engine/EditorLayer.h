#pragma once

#include "SceneHierarchyPanel.h"
#include "ContentBrowserPanel.h"
#include "OrthographicCameraController.h"
#include "EditorCamera.h"
#include "Layer.h"
#include "KeyEvent.h"

namespace SY {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(float timestep) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void OnOverlayRender();

		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();
		void SetCurrentSceneAsEntry();

		void SerializeScene(shared_ptr<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();
		void OnScenePause();

		void OnDuplicateEntity();

		void UI_Toolbar();
	private:
		//SY::OrthographicCameraController m_CameraController;

		shared_ptr<Scene> m_ActiveScene;
		shared_ptr<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		Entity m_HoveredEntity;

		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;

		shared_ptr<Texture> m_CheckerboardTexture;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		Vector2 m_ViewportSize = { 0.0f, 0.0f };
		Vector2 m_ViewportBounds[2];

		Vector4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		int m_GizmoType = -1;
		bool m_ShowPhysicsColliders = false;
		bool m_ShowImguiPanels = true;

		float CameraSpeed = 0.1f;
		float CameraWheelStep = 1.f;

		int CollisionLayers = 1;
		string LayerName[16];

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		unique_ptr<ContentBrowserPanel> m_ContentBrowserPanel;
		shared_ptr<Texture> m_EntityTex;
		// Editor resources
		shared_ptr<Texture> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop;
	};

}