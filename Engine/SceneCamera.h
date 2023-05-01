#pragma once
#include "Camera.h"

namespace SY {

	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); }

		float GetNearClip() const { return m_Near; }
		void SetNearClip(float nearClip) { m_Near = nearClip; RecalculateProjection(); }
		float GetFarClip() const { return m_Far; }
		void SetFarClip(float farClip) { m_Far = farClip; RecalculateProjection(); }

		void SetOrthographicSize(float orthographicSize) { m_OrthographicSize = orthographicSize; RecalculateProjection(); }
		float GetOrthographicSize() const { return m_OrthographicSize; }

		void SetAspectRatio(float ratio) { m_AspectRatio = ratio; }
		float GetAspectRatio() { return m_AspectRatio; }

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }
	private:
		void RecalculateProjection();
	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_PerspectiveFOV = XM_PI / 4.f;
		float m_Near = 0.01f, m_Far = 1000.0f;

		float m_OrthographicSize = 10.0f;

		float m_AspectRatio = 16.f/9.f;
	};

}
