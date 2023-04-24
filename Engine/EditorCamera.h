#pragma once

#include "Camera.h"

namespace SY {
	class Event;
	class MouseScrolledEvent;
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(float timeStep);
		void OnEvent(Event& e);
		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const Matrix& GetView() const { return m_ViewMatrix; }
		const Vector3& GetPosition() const { return m_Position; }

		void SetMoveSpeed(float speed) { m_CameraSpeed = speed; }
		void SetWheelStep(float step) { m_ZoomStep = step; }

	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void Translate(Vector3 delta);
		void Rotate(Quaternion rotation);

	private:

		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		float m_ViewportWidth;
		float m_ViewportHeight;
		Matrix m_ViewMatrix;

		Vector3 m_Position = Vector3(0,0,-10.f);

		float m_CameraSpeed = 0.1f;
		float m_ZoomStep = 1.f;

		Quaternion m_Rotation = Quaternion::Identity;
	};

}