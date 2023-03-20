#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"

Transform::Transform()
	:Component(Component_Type::Transform)
	,_position(Vector3::Zero)
	,_rotation(Quaternion::Identity)
	,_scale(Vector3::One)
	,_parent{}
	,_fixed(false)
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

	Vector3 a = Vector3::Transform(Vector3::Zero, _world);
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

void Transform::Translate(const Vector3& worldDir)
{
	if (!_fixed)
	{
		shared_ptr<Transform> parent = _parent.lock();
		if (_parent.lock())
			_position += Vector3::Transform(worldDir, parent->GetWorld().Invert());
		else
			_position += worldDir;
	}
}
