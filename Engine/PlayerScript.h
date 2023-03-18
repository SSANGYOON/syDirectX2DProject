#pragma once
#include "Script.h"

class Collider;
class Animator;
class BaseRenderer;
class RigidBody;
class PlayerScript : public Script
{
public:
	PlayerScript();
	virtual ~PlayerScript();

	virtual void Start() override;
	virtual void Update() override;

	void jump();
	void Move();
	void croutch();
	void roll();

	virtual void OntriggerEnter(Collider* collider) override;
	virtual void OntriggerExit(Collider* collider) override;
	virtual void OnCollisionEnter(Collider* collider) override;
	virtual void OnCollisionStay(Collider* collider) override;
	virtual void OnCollisionExit(Collider* collider) override;

public:
	enum PlayerState : UINT8
	{
		GROUND,
		CANJUMP,
		CROUTCH,
		GUARD,
		BLOCKED,
		ATTACK,
		ATTACKED,
		ROLLING,
		DEAD,
		END
	};

	bitset<(size_t)PlayerState::END> stateBit;

private:
	shared_ptr<Animator> animater;
	shared_ptr<RigidBody> rigidBody;
	shared_ptr<BaseRenderer> renderer;
	const Vector3 jumpForce;
	const float moveSpeed;
	Vector3 _accel = Vector3::Zero;
	float _smoothTime = 0.05f;
};

