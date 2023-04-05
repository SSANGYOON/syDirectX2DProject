#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"
#include "GameObject.h"
Transform::Transform(GameObject* owner)

	:Component(Component_Type::Transform, owner)
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

	if (_parent != nullptr)
		_world *= _parent->GetWorld();

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
	cb->SetPipline(ShaderStage::GS);
	cb->SetPipline(ShaderStage::PS);
}

void Transform::Translate(const Vector3& worldDir)
{
	if (!_fixed)
	{
		if (_parent)
			_position += Vector3::Transform(worldDir, _parent->GetWorld().Invert());
		else
			_position += worldDir;
	}
}

const Vector3 Transform::GetWorldPosition()
{
	return _world.Translation();
}

const Vector3 Transform::GetLocalToWorld(Vector3 localPos)
{
	return Vector3::Transform(localPos, _world);
}

Transform* Transform::GetChild(const wstring& name)
{
	auto it = _children.find(name);
	if (it != _children.end())
		return it->second;
	else
		return nullptr;
}

void Transform::SetChild(Transform* child, const wstring& name)
{
	_children[name] = child;
	child->_parent = this;
}
