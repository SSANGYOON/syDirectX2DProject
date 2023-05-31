#pragma once

namespace SY {
	class Camera
	{
	public:
		Camera() = default;
		Camera(const Matrix& projection)
			: m_Projection(projection) {}

		virtual ~Camera() = default;
		const Matrix& GetProjection() const { return m_Projection; }

	protected:
		Matrix m_Projection = Matrix::Identity;
	};
}
