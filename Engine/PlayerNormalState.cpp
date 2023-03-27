#include "pch.h"
#include "PlayerNormalState.h"
#include "Player.h"
#include "FSM.h"
#include "Input.h"

PlayerNormalState::PlayerNormalState(Player* player, FSM* stateMachine, const wstring& stateName)
	:PlayerState(player, stateMachine, stateName)
{
}

void PlayerNormalState::Enter()
{
	PlayerState::Enter();
	_controllable = true;
}

void PlayerNormalState::Exit()
{
	_controllable = false;
}



void PlayerNormalState::CheckTransition()
{
	if(_player->_attacked) {
		_fsm->ChangeState(_player->attacked.get(), FSM::READY);
		isExitingState = true;
	}

	else if (INPUT->GetKeyState(KEY_TYPE::C) == KEY_STATE::DOWN) {
		_fsm->ChangeState(_player->roll.get(), FSM::READY);
		isExitingState = true;
	}

	else if (_player->GetWeapon(0) && INPUT->GetKeyState(KEY_TYPE::Z) == KEY_STATE::PRESS)
	{
		_fsm->ChangeState(_player->attack1.get(), FSM::READY);
		isExitingState = true;
	}

}

