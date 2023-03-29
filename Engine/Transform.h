#pragma once
#include "Component.h"
#include "GameObject.h"
class Transform : public Component
{
public:
	Transform(GameObject* owner);
	virtual ~Transform();

	virtual void FinalUpdate() override;
	void SetTransformBuffer();

public:
	void SetParent(Transform* parent) { _parent = parent; }
	Transform* GetParent() { return _parent; }

	void Translate(const Vector3& localDir);
	const Vector3& GetLocalPosition() { return _position; }
	const Quaternion& GetRotation() { return _rotation; }
	const Vector3& GetScale() { return _scale; }
	const Vector3 GetWorldPosition();
	const Vector3 GetLocalToWorld(Vector3 localPos);
	const Matrix& GetWorld() { return _world; }

	void SetPosition(const Vector3& position) { _position = position; };
	void SetRotation(const Quaternion& rotation) { _rotation = rotation; };
	void SetScale(const Vector3& scale) { _scale = scale; };
	void SetFixed(bool fixed) { _fixed = fixed; }

	Transform* GetChild(const wstring& name);
	const map<wstring, Transform*>& GetChildren() { return _children; }
	void SetChild(Transform* child, const wstring& name);

private:
	friend class Transform;
	Transform* _parent;
	map<wstring, Transform*> _children;
	Vector3 _position;
	Quaternion _rotation;
	Vector3 _scale;
	Matrix _world;
	bool _fixed;
};

