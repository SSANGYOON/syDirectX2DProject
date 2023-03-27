#include "pch.h"
#include "PlayerActiveState.h"
#include "FSM.h"

PlayerActiveState::PlayerActiveState(Player* player, FSM* stateMachine, const wstring& stateName)
	:PlayerState(player, stateMachine, stateName)
{
}

void PlayerActiveState::Enter()
{
	PlayerState::Enter();
}

void PlayerActiveState::Exit()
{
}

void PlayerActiveState::CheckTransition()
{
	if (_player->_attacked) {
		_fsm->ChangeState(_player->attacked.get(), FSM::READY);
		isExitingState = true;
	}
}

