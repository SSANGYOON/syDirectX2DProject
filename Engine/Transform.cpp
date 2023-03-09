#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"

Transform::Transform()
	:Component(Component_Type::Transform)
	,_position(Vector3::Zero)
	,_rotation(Quaternion::Identity)
	,_scale(Vector3::One)
	, _parent{}
{

}

Transform::~Transform()
{
}

void Transform::FinalUpdate()
{
	Matrix matScale = Matrix::CreateScale(_scale);
	Matrix matRotation = Matrix::CreateFromQuaternion(_rotation);
	Matrix matTranslation = Matrix::CreateTranslation(_position);
	_world = matScale * matRotation * matTranslation;
	auto parent = _parent.lock();
	if (parent != nullptr)
		_world *= parent->GetWorld();

}

void Transform::SetTransformBuffer()
{
	TransformCB trCB = {};
	trCB.world = _world;
	trCB.view = Camera::s_view;
	trCB.projection = Camera::s_projection;
	shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::TRANSFORM);
	cb->SetData(&trCB);
	cb->SetPipline(ShaderStage::VS);
}
