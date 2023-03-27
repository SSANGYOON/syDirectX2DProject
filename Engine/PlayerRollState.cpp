#include "pch.h"
#include "PlayerRollState.h"
#include "FSM.h"
#include "Animator.h"

PlayerRollState::PlayerRollState(Player* player, FSM* stateMachine, const wstring& stateName)
	:PlayerActiveState(player, stateMachine, stateName)
{
}

void PlayerRollState::Update()
{
	PlayerState::Update();
	CheckTransition();
	
	if (isExitingState)
		return;
	if (elapsedTime > 0.9f)
		_player->_invisible = false;
	else if (elapsedTime > 0.1f)
		_player->_invisible = true;
}

void PlayerRollState::Enter()
{
	PlayerActiveState::Enter();
}

void PlayerRollState::Exit()
{
	PlayerActiveState::Exit();
	_player->_invisible = false;
}

void PlayerRollState::CheckTransition()
{
	PlayerActiveState::CheckTransition();

	if (isExitingState)
		return;

	if (elapsedTime > 1.f) {
		_fsm->ChangeState(_player->idle.get(), FSM::READY);
		isExitingState = true;
	}	
}