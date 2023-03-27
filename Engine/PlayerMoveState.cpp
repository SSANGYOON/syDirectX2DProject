#include "pch.h"
#include "PlayerMoveState.h"
#include "Animator.h"
#include "FSM.h"
#include "Input.h"

PlayerMoveState::PlayerMoveState(Player* player, FSM* stateMachine, const wstring& stateName)
	:PlayerNormalState(player, stateMachine, stateName)
{
}

void PlayerMoveState::Update()
{
	PlayerState::Update();
	CheckTransition();
}

void PlayerMoveState::Enter()
{
	PlayerNormalState::Enter();
}

void PlayerMoveState::Exit()
{
	PlayerNormalState::Exit();
}

void PlayerMoveState::CheckTransition()
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

	else if (INPUT->GetKeyState(KEY_TYPE::LEFT) == KEY_STATE::NONE && INPUT->GetKeyState(KEY_TYPE::RIGHT) == KEY_STATE::NONE) {
		_fsm->ChangeState(_player->idle.get(), FSM::YET);
		_player->GetAnimator()->Play(L"RUN2IDLE");
	}

	else
		isExitingState = false;
}