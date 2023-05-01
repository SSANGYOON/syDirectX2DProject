#include "pch.h"
#include "OrthographicCameraController.h"
#include "ApplicationEvent.h"
#include "MouseEvent.h"

namespace SY {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
	{
	}

	void OrthographicCameraController::OnUpdate(float timeStep)
	{

		if (KEY_PRESSED(KEY_TYPE::A))
		{
			m_CameraPosition.x -= cos(m_CameraRotation / 180.f * XM_PI) * m_CameraTranslationSpeed * timeStep;
			m_CameraPosition.y -= sin(m_CameraRotation / 180.f * XM_PI) * m_CameraTranslationSpeed * timeStep;
		}
		else if (KEY_PRESSED(KEY_TYPE::D))
		{
			m_CameraPosition.x += cos(m_CameraRotation / 180.f * XM_PI) * m_CameraTranslationSpeed * timeStep;
			m_CameraPosition.y += sin(m_CameraRotation / 180.f * XM_PI) * m_CameraTranslationSpeed * timeStep;
		}

		if (KEY_PRESSED(KEY_TYPE::W))
		{
			m_CameraPosition.x += -sin(m_CameraRotation / 180.f * XM_PI) * m_CameraTranslationSpeed * timeStep;
			m_CameraPosition.y += cos(m_CameraRotation / 180.f * XM_PI) * m_CameraTranslationSpeed * timeStep;
		}
		else if (KEY_PRESSED(KEY_TYPE::S))
		{
			m_CameraPosition.x -= -sin(m_CameraRotation / 180.f * XM_PI) * m_CameraTranslationSpeed * timeStep;
			m_CameraPosition.y -= cos(m_CameraRotation / 180.f * XM_PI) * m_CameraTranslationSpeed * timeStep;
		}

		if (m_Rotation)
		{
			if (KEY_PRESSED(KEY_TYPE::Q))
				m_CameraRotation += m_CameraRotationSpeed * timeStep;
			if (KEY_PRESSED(KEY_TYPE::E))
				m_CameraRotation -= m_CameraRotationSpeed * timeStep;

			if (m_CameraRotation > 180.0f)
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);

		m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

}