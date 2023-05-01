#include "pch.h"
#include "OrthographicCamera.h"

namespace SY {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, 1.0f, 1000.0f)), m_ViewMatrix(Matrix::Identity)
	{

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{

		m_ProjectionMatrix = ::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, 1.0f, 1000.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{

		Matrix transform = Matrix::CreateTranslation(m_Position) *
			Matrix::CreateFromYawPitchRoll(Vector3(0,0,m_Rotation / 180.f * XM_PI));

		m_ViewMatrix = transform.Invert();
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}