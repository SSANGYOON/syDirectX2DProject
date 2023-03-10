#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "GameObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Engine.h"

Matrix Camera::s_view;
Matrix Camera::s_projection;

Camera::Camera()
	:Component(Component_Type::Camera)
	, _fov(XM_PI/3)
	, _type(ProjectionType::Orthographic)
	, _aspectRatio(1.0f)
	, _near(1.0f)
	, _far(1000.0f)
	, _scale(1.0f)
{
	EnableLayerMasks();
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
	CreateView();
	CreateProjection();
}

void Camera::CreateView()
{
	shared_ptr<Transform> tr = static_pointer_cast<Transform>(_owner.lock()->GetComponent(Component_Type::Transform));
	const Matrix& m = tr->GetWorld();
	_view = m.Invert();
}

void Camera::CreateProjection()
{
	WindowInfo info = GEngine->GetWindow();
	if (_type == ProjectionType::Perspective)
		_projection = ::XMMatrixPerspectiveFovLH(_fov, _aspectRatio, _near, _far);
	else
		_projection = ::XMMatrixOrthographicLH(info.width * _scale, info.height * _scale, _near, _far);
}

void Camera::TurnLayerMask(UINT layerIndex, bool enable)
{
	_layerMasks.set(layerIndex, enable);
}

void Camera::Render()
{
	Camera::s_projection = _projection;
	Camera::s_view = _view;

	shared_ptr<Scene> curScene = GET_SINGLE(SceneManager)->GetActiveScene();

	for (size_t i = 0; i < 32; i++)
	{
		if (_layerMasks[i])
		{
			Layer& layer = curScene->GetLayer((LAYER_TYPE)i);
			layer.Render();
		}
	}

}
