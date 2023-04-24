#pragma once

namespace SY {

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		const Vector3& GetPosition() const { return m_Position; }
		void SetPosition(const Vector3& position) { m_Position = position; RecalculateViewMatrix(); }

		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		const Matrix& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const Matrix& GetViewMatrix() const { return m_ViewMatrix; }
		const Matrix& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();
	private:
		Matrix m_ProjectionMatrix;
		Matrix m_ViewMatrix;
		Matrix m_ViewProjectionMatrix;

		Vector3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};

}