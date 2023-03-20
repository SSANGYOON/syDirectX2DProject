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
	void crouch();
	void roll();
	void Attacked();
	void Invisible() { _invisible = true; }
	void Vulnerable() { _invisible = false; }

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
	void Flip();

	void RollingEnd();
	void AttackedEnd();
	bool _facingRight;
	bool _canJump;
	bool _ground;
	bool _crouching;
	bool _rolling;
	bool _attacked;
	bool _invisible;

	shared_ptr<Animator> animater;
	shared_ptr<RigidBody> rigidBody;
	shared_ptr<BaseRenderer> renderer;
	shared_ptr<Transform> transform;

	
	const Vector3 jumpForce;
	const float moveSpeed;

	const float evadeSpeed;
	const float evadeTime;
	float evadeElapsed;

	Vector3 _accel = Vector3::Zero;
	Vector3 _velocity = Vector3::Zero;
	float _smoothTime = 0.1f;
};

