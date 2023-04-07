#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "GameObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Engine.h"

Matrix Camera::s_view;
Matrix Camera::s_projection;

Camera::Camera(GameObject* owner)
	:Component(Component_Type::Camera, owner)
	, _fov(XM_PI/3)
	, _type(ProjectionType::Orthographic)
	, _aspectRatio(1.0f)
	, _near(1.0f)
	, _far(1000.0f)
	, _scale(1.0f)
	, _width(640.f)
	, _height(384.f)
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
	Transform* tr = _owner->GetTransform();
	const Matrix& m = tr->GetWorld();
	_view = m.Invert();
}

void Camera::CreateProjection()
{
	if (_type == ProjectionType::Perspective)
		_projection = ::XMMatrixPerspectiveFovLH(_fov, _aspectRatio, _near, _far);
	else
		_projection = ::XMMatrixOrthographicLH(_width * _scale, _height * _scale, _near, _far);
}

void Camera::TurnLayerMask(UINT layerIndex, bool enable)
{
	_layerMasks.set(layerIndex, enable);
}

void Camera::Render()
{
	Camera::s_projection = _projection;
	Camera::s_view = _view;


	Scene* curScene = GET_SINGLE(SceneManager)->GetActiveScene();

	for (size_t i = 0; i < 32; i++)
	{
		if (_layerMasks[i])
		{
			Layer& layer = curScene->GetLayer((LAYER_TYPE)i);
			layer.Render();
		}
	}

}
