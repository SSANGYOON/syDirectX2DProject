#include "pch.h"
#include "SceneCamera.h"

namespace SY {

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_Near = nearClip;
		m_Far = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_Near = nearClip;
		m_Far = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		assert(width > 0 && height > 0);
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
			m_Projection = Matrix::CreatePerspectiveFieldOfViewLH(m_PerspectiveFOV, m_AspectRatio, m_Near, m_Far);

		else
			m_Projection = Matrix::CreateOrthographicLH(m_OrthographicSize * m_AspectRatio, m_OrthographicSize, m_Near, m_Far);
	}

	float CameraComponent::CalculateDiff(float ts, float accTime)
	{
		oscillationAmp = max(0.f, oscillationAmp - ts * oscillationDamp);

		return oscillationAmp * sinf(XM_PI * 2 * oscillationFreq * accTime);
	}

}