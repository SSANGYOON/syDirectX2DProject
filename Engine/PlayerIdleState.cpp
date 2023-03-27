#include "pch.h"
#include "PlayerIdleState.h"
#include "Animator.h"
#include "FSM.h"
#include "Input.h"

PlayerIdleState::PlayerIdleState(Player* player, FSM* stateMachine, const wstring& stateName)
	:PlayerNormalState(player, stateMachine, stateName)
{
}

void PlayerIdleState::Update()
{
	PlayerState::Update();
	CheckTransition();
}

void PlayerIdleState::Enter()
{
	PlayerNormalState::Enter();
}

void PlayerIdleState::Exit()
{
	PlayerNormalState::Exit();
}

void PlayerIdleState::CheckTransition()
{
	PlayerNormalState::CheckTransition();

	if (isExitingState)
		return;

	isExitingState = true;

	if (!_player->_ground) {
		if(_player->_falling)
			_fsm->ChangeState(_player->fall.get(), FSM::READY);
		else
			_fsm->ChangeState(_player->jump.get(), FSM::READY);
	}
	else if (INPUT->GetKeyState(KEY_TYPE::RIGHT) == KEY_STATE::PRESS || INPUT->GetKeyState(KEY_TYPE::LEFT) == KEY_STATE::PRESS) {
		_fsm->ChangeState(_player->move.get(), FSM::YET);
		_player->GetAnimator()->Play(L"IDLE2RUN");
	}
	else if (INPUT->GetKeyState(KEY_TYPE::DOWN) == KEY_STATE::PRESS) {
		_fsm->ChangeState(_player->crouch.get(), FSM::YET);
		_player->GetAnimator()->Play(L"CROUCH");
	}
	else
		isExitingState = false;
}
