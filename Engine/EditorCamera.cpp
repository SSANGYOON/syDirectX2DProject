#include "pch.h"
#include "EditorCamera.h"
#include "MouseEvent.h"

namespace SY {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), Camera()
	{
		UpdateView();
	}

	void EditorCamera::UpdateProjection()
	{
		m_Projection = ::XMMatrixPerspectiveFovLH(m_FOV / 180.f * XM_PI, m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView()
	{
		m_ViewMatrix = Matrix::CreateTranslation(m_Position) * Matrix::CreateFromQuaternion(m_Rotation);
		m_ViewMatrix = m_ViewMatrix.Invert();
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		m_Position.z += delta;
		return false;
	}

	void EditorCamera::Translate(Vector3 delta)
	{
		m_Position += delta;
	}

	void EditorCamera::Rotate(Quaternion rotation)
	{
		m_Rotation = rotation * m_Rotation;
	}

	void EditorCamera::OnUpdate(float timeStep)
	{
		Vector3 delta = Vector3::Zero;

		if (KEY_PRESSED(KEY_TYPE::UP))
			delta.y += m_CameraSpeed * timeStep;
		if (KEY_PRESSED(KEY_TYPE::DOWN))
			delta.y -= m_CameraSpeed * timeStep;
		if (KEY_PRESSED(KEY_TYPE::LEFT))
			delta.x -= m_CameraSpeed * timeStep;
		if (KEY_PRESSED(KEY_TYPE::RIGHT))
			delta.x += m_CameraSpeed * timeStep;
		Translate(delta);

		UpdateView();
		UpdateProjection();
	}
	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}
}