#include "pch.h"
#include "PlayerCrouchState.h"
#include "FSM.h"
#include "Animator.h"
#include "Input.h"
#include "Collider2D.h"

PlayerCrouchState::PlayerCrouchState(Player* player, FSM* stateMachine, const wstring& stateName) :
	PlayerNormalState(player, stateMachine, stateName)
{
}

void PlayerCrouchState::Update()
{
	PlayerState::Update();
	CheckTransition();
}

void PlayerCrouchState::Enter()
{
	PlayerNormalState::Enter();
	_controllable = false;
	_player->collider->SetSize(Vector3(1.f, 2.5f, 1.f));
	_player->collider->SetLocalCenter(Vector3(0.f, -0.5f, 0.f));
}

void PlayerCrouchState::Exit()
{
	PlayerNormalState::Exit();
	_player->collider->SetSize(Vector3(1.f, 4.0f, 1.f));
	_player->collider->SetLocalCenter(Vector3(0.f, -0.5f, 0.f));
}

void PlayerCrouchState::CheckTransition()
{
	PlayerNormalState::CheckTransition();

	if (isExitingState)
		return;

	isExitingState = true;

	if (!_player->_ground) {
		if (_player->_falling)
			_fsm->ChangeState(_player->fall.get(), FSM::READY);
		else
			_fsm->ChangeState(_player->jump.get(), FSM::READY);
	}

	else if (INPUT->GetKeyState(KEY_TYPE::DOWN) != KEY_STATE::PRESS) {
		_fsm->ChangeState(_player->idle.get(), FSM::YET);
		_player->GetAnimator()->Play(L"CROUCH2IDLE");
	}
	else
		isExitingState = false;
}
