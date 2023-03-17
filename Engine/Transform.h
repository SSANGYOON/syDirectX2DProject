#pragma once
#include "Component.h"
class Transform : public Component
{
public:
	Transform();
	virtual ~Transform();

	virtual void FinalUpdate() override;
	void SetTransformBuffer();

public:
	void SetParent(shared_ptr<Transform> parent) { _parent = parent; }
	shared_ptr<Transform> GetParent() { return _parent.lock(); }

	void Translate(const Vector3& localDir);
	const Vector3& GetPosition() { return _position; }
	const Quaternion& GetRotation() { return _rotation; }
	const Vector3& GetScale() { return _scale; }
	const Matrix& GetWorld() { return _world; }

	void SetPosition(const Vector3& position) { _position = position; };
	void SetRotation(const Quaternion& rotation) { _rotation = rotation; };
	void SetScale(const Vector3& scale) { _scale = scale; };
	void SetFixed(bool fixed) { _fixed = fixed; }

private:
	weak_ptr<Transform> _parent;

	Vector3 _position;
	Quaternion _rotation;
	Vector3 _scale;
	Matrix _world;
	bool _fixed;
};

