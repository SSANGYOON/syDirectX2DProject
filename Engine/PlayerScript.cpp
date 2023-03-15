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
	: Script(), jumpForce(Vector3(0.f,5.f,0.f)), moveForce(1.0f)
{
}

PlayerScript::~PlayerScript()
{
}

void PlayerScript::Start()
{
	shared_ptr<GameObject> gameObject = GetOwner();
	animater = static_pointer_cast<Animator>(gameObject->GetComponent(Component_Type::Animator));
	rigidBody = static_pointer_cast<RigidBody>(gameObject->GetComponent(Component_Type::RigidBody));
	renderer = static_pointer_cast<SpriteRenderer>(gameObject->GetComponent(Component_Type::Renderer));
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
	if (stateBit.none())
		if (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::SPACE) == KEY_STATE::PRESS)
		{
			rigidBody->ApplyForce(jumpForce);
			stateBit[(size_t)PlayerState::AERIAL] = true;
		}
}

void PlayerScript::Move()
{
	if (stateBit.to_ulong() < 4)
	{
		if(GET_SINGLE(Input)->GetKeyState(KEY_TYPE::RIGHT) == KEY_STATE::DOWN)
			rigidBody->ApplyForce(Vector3(moveForce, 0.f, 0.f));
		if (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::LEFT) == KEY_STATE::DOWN)
			rigidBody->ApplyForce(Vector3(-moveForce, 0.f, 0.f));
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

void PlayerScript::OntriggerStay(Collider* collider)
{

}

void PlayerScript::OnCollisionEnter(Collider* collider)
{
}

void PlayerScript::OnCollisionStay(Collider* collider)
{

}

void PlayerScript::OnCollisionExit(Collider* collider)
{
}
