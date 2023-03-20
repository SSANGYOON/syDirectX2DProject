#include "pch.h"
#include "PlayerScript.h"
#include "GameObject.h"
#include "Collider.h"
#include "RigidBody.h"
#include "Animator.h"
#include "SpriteRenderer.h"
#include "Input.h"
#include "Timer.h"

PlayerScript::PlayerScript()
	: Script(), _facingRight(true), _ground(false), _canJump(false), _crouching(false), _rolling(false), _attacked(false), _invisible(false)
	,evadeTime(1.f), evadeSpeed(15.f), evadeElapsed(0.f), jumpForce(Vector3(0.f, 20.f, 0.f)), moveSpeed(10.0f)
{
}

PlayerScript::~PlayerScript()
{
}

void PlayerScript::Start()
{
	shared_ptr<GameObject> gameObject = GetOwner();
	animater = gameObject->GetComponent<Animator>();
	animater->AddFloatCondition(L"VelocityX", &_velocity.x);
	animater->AddFloatCondition(L"VelocityY", &_velocity.y);
	animater->AddBooleanCondition(L"ground", &_ground);
	animater->AddBooleanCondition(L"canJump", &_canJump);
	animater->AddBooleanCondition(L"crouching", &_crouching);
	animater->AddBooleanCondition(L"rolling", &_rolling);
	animater->AddBooleanCondition(L"attacked", &_attacked);

	vector<Animator::Condition> cons = { { Animator::ConditionType::BOOL, L"attacked", true } };
	animater->AddTransitionRule(cons, L"IDLE", L"ATTACKED");
	animater->AddTransitionRule(cons, L"JUMP", L"ATTACKED");
	animater->AddTransitionRule(cons, L"FALLING", L"ATTACKED");
	animater->AddTransitionRule(cons, L"RUN", L"ATTACKED");
	animater->AddTransitionRule(cons, L"RUN2IDLE", L"ATTACKED");
	animater->AddTransitionRule(cons, L"IDLE2RUN", L"ATTACKED");
	cons = { { Animator::ConditionType::FLOAT_ABS_GREATER, L"VelocityX", 1.f }};
	animater->AddTransitionRule(cons, L"IDLE", L"IDLE2RUN");
	cons = { { Animator::ConditionType::FLOAT_ABS_LESS, L"VelocityX", 1.f } };
	animater->AddTransitionRule(cons, L"RUN", L"RUN2IDLE");
	cons = { { Animator::ConditionType::FLOAT_ABS_GREATER, L"VelocityX", 1.f } };
	animater->AddTransitionRule(cons, L"RUN2IDLE", L"RUN");
	cons = { { Animator::ConditionType::FLOAT_GREATER, L"VelocityY", 5.f } };
	animater->AddTransitionRule(cons, L"RUN2IDLE", L"JUMP");
	animater->AddTransitionRule(cons, L"IDLE", L"JUMP");
	animater->AddTransitionRule(cons, L"RUN", L"JUMP");
	animater->AddTransitionRule(cons, L"IDLE2RUN", L"JUMP");
	cons = { { Animator::ConditionType::FLOAT_LESS, L"VelocityY", 0.f } };
	animater->AddTransitionRule(cons, L"JUMP", L"FALLING");
	cons = { { Animator::ConditionType::BOOL, L"ground", true } };
	animater->AddTransitionRule(cons, L"FALLING", L"LANDING");
	cons = { { Animator::ConditionType::BOOL, L"crouching", true },{ Animator::ConditionType::FLOAT_ABS_LESS, L"VelocityX" , 0.5f}};
	animater->AddTransitionRule(cons, L"IDLE", L"CROUCH");
	cons = { { Animator::ConditionType::BOOL, L"crouching", false } };
	animater->AddTransitionRule(cons, L"CROUCHING", L"CROUCH2IDLE");
	cons = { { Animator::ConditionType::BOOL, L"rolling", true } };
	animater->AddTransitionRule(cons, L"IDLE", L"ROLLING");
	animater->AddTransitionRule(cons, L"RUN", L"ROLLING");

	shared_ptr<AnimationClip> clip = animater->GetClip(L"ROLLING");
	clip->SetCompleteEvent(bind(&PlayerScript::RollingEnd, this));
	clip->addEvent(0.f,bind(&PlayerScript::Invisible, this));
	clip->addEvent(0.875f, bind(&PlayerScript::Vulnerable, this));

	clip = animater->GetClip(L"ATTACKED");
	clip->SetCompleteEvent(bind(&PlayerScript::AttackedEnd, this));

	cons = { { Animator::ConditionType::BOOL, L"canJump", true } };
	animater->AddTransitionRule(cons, L"ATTACKED", L"IDLE");

	rigidBody = gameObject->GetComponent<RigidBody>();
	renderer = gameObject->GetComponent<BaseRenderer>();
	transform = gameObject->GetComponent<Transform>();
	stateBit.reset();
};
	

void PlayerScript::Update()
{
	jump();
	Move();
	crouch();
	roll();
}

void PlayerScript::jump()
{
	Vector3 velocity = rigidBody->GetVelocity();

	if (_ground && _canJump && !_crouching && !_rolling)
	{
		if (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::SPACE) == KEY_STATE::DOWN)
		{ 
			rigidBody->ApplyForce(jumpForce, RigidBody::IMPULSE);
			_canJump = false;
		}
	}
}

void PlayerScript::Move()
{
	if (_attacked)
		return;

	_velocity = rigidBody->GetVelocity();
	if (_crouching)
		rigidBody->SetVelocity(RigidBody::SmoothDamp(_velocity, Vector3(0, _velocity.y, _velocity.z), _accel, _smoothTime));
	else if (_rolling)
	{
		evadeElapsed += GET_SINGLE(Timer)->DeltaTime();
		float delta = evadeElapsed / evadeTime;
		float targetSpeed = (1.f - delta) * (evadeSpeed - moveSpeed / 2.f) + delta * moveSpeed / 2.f;
		if(_facingRight)
			rigidBody->SetVelocity({targetSpeed, _velocity.y, _velocity.x});
		else
			rigidBody->SetVelocity({-targetSpeed, _velocity.y, _velocity.x });
	}
		
	else {
		if (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::RIGHT) == KEY_STATE::PRESS)
		{
			if (!_facingRight)
				Flip();
			rigidBody->SetVelocity(RigidBody::SmoothDamp(_velocity, Vector3(moveSpeed, _velocity.y, _velocity.z), _accel, _smoothTime));
		}
		else if (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::LEFT) == KEY_STATE::PRESS)
		{
			if (_facingRight)
				Flip();
			rigidBody->SetVelocity(RigidBody::SmoothDamp(_velocity, Vector3(-moveSpeed, _velocity.y, _velocity.z), _accel, _smoothTime));
		}
		else
			rigidBody->SetVelocity(RigidBody::SmoothDamp(_velocity, Vector3(0, _velocity.y, _velocity.z), _accel, _smoothTime));
	}

	_velocity = rigidBody->GetVelocity();
}

void PlayerScript::crouch()
{
	if (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::DOWN) == KEY_STATE::DOWN)
		_crouching = true;
	else if(GET_SINGLE(Input)->GetKeyState(KEY_TYPE::DOWN) == KEY_STATE::UP)
		_crouching = false;
}

void PlayerScript::roll()
{
	if (!_crouching && _ground && (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::C) == KEY_STATE::DOWN))
	{
		_velocity = Vector3::Zero;
		_rolling = true;
	}
}

void PlayerScript::Attacked()
{
	_canJump = false;
	_rolling = false;
	_crouching = false;
	_attacked = true;
	if (_facingRight)
		_velocity = Vector3(-7.f, 7.f, 0.f);
	else
		_velocity = Vector3(7.f, 7.f, 0);
	rigidBody->SetVelocity(_velocity);
}

void PlayerScript::RollingEnd()
{
	_rolling = false;
	evadeElapsed = 0.f;
}

void PlayerScript::AttackedEnd()
{
	_attacked = false; 
}

void PlayerScript::OntriggerEnter(Collider* collider)
{
	shared_ptr<GameObject> opponent = collider->GetOwner();
	shared_ptr<Transform> opponentTr = opponent->GetTransform();
	LAYER_TYPE type = opponent->GetType();
	if(type == LAYER_TYPE::FIXEDOBJECT)
		_canJump = true;
	if (type == LAYER_TYPE::MONSTER && !_attacked && !_invisible)
	{
		Vector3 diff = opponentTr->GetPosition() - transform->GetPosition();
		if (_facingRight != diff.x > 0)
			Flip();
		Attacked();
	}
}

void PlayerScript::OntriggerExit(Collider* collider)
{
	shared_ptr<GameObject> opponent = collider->GetOwner();
	LAYER_TYPE type = opponent->GetType();
	if (type == LAYER_TYPE::FIXEDOBJECT)
	_ground = false;
}

void PlayerScript::OnCollisionEnter(Collider* collider)
{
	shared_ptr<GameObject> opponent = collider->GetOwner();
	shared_ptr<Transform> opponentTr = opponent->GetTransform(); 
	LAYER_TYPE type = opponent->GetType();
	if (type == LAYER_TYPE::FIXEDOBJECT)
		_ground = true;
	
}

void PlayerScript::OnCollisionStay(Collider* collider)
{

}

void PlayerScript::OnCollisionExit(Collider* collider)
{
}

void PlayerScript::Flip()
{
	_facingRight = !_facingRight;

	Vector3 theScale = transform->GetScale();
	theScale.x *= -1.f;
	transform->SetScale(theScale);
}