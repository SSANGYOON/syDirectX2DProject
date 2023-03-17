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
	: Script(), jumpForce(Vector3(0.f,5.f,0.f)), moveSpeed(10.0f)
{
}

PlayerScript::~PlayerScript()
{
}

void PlayerScript::Start()
{
	shared_ptr<GameObject> gameObject = GetOwner();
	animater = gameObject->GetComponent<Animator>();
	rigidBody = gameObject->GetComponent<RigidBody>(); 
	renderer = gameObject->GetComponent<BaseRenderer>();
	stateBit.reset();
}

void PlayerScript::Update()
{

	jump();
	Move();
	croutch();
}

void PlayerScript::jump()
{
	Vector3 velocity = rigidBody->GetVelocity();
	if (stateBit.to_ulong() == 3)
		if (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::SPACE) == KEY_STATE::PRESS)
		{
			rigidBody->SetVelocity(Vector3(velocity.x, 300.f, 0.f));
			stateBit[(size_t)PlayerState::CANJUMP] = false;
		}
}

void PlayerScript::Move()
{
	if (stateBit.to_ulong() < 4)
	{
		Vector3 velocity = rigidBody->GetVelocity();
		if (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::RIGHT) == KEY_STATE::PRESS)
			rigidBody->SetVelocity(Vector3(300.f, velocity.y, 0.f));

		else if (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::LEFT) == KEY_STATE::PRESS)
			rigidBody->SetVelocity(Vector3(-300.f, velocity.y, 0.f));

		else
			rigidBody->SetVelocity(Vector3(0, velocity.y, 0.f));
		int a = 0;
	}
}

void PlayerScript::croutch()
{
	if (stateBit.none() && (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::DOWN) == KEY_STATE::PRESS))
	{
		stateBit[(size_t)PlayerState::CROUTCH] = true;
	}
	else if(stateBit.to_ulong() > 1|| (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::C) == KEY_STATE::UP))
		stateBit[(size_t)PlayerState::CROUTCH] = false;
}

void PlayerScript::roll()
{
	if (stateBit.none() && (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::C) == KEY_STATE::PRESS))
	{
		stateBit[(size_t)PlayerState::CROUTCH] = true;
	}
}

void PlayerScript::OntriggerEnter(Collider* collider)
{
	stateBit[(size_t)PlayerState::CANJUMP] = true;
}

void PlayerScript::OntriggerExit(Collider* collider)
{
	stateBit[(size_t)PlayerState::GROUND] = false;
}

void PlayerScript::OnCollisionEnter(Collider* collider)
{
	stateBit[(size_t)PlayerState::GROUND] = true;
}

void PlayerScript::OnCollisionStay(Collider* collider)
{

}

void PlayerScript::OnCollisionExit(Collider* collider)
{
}
