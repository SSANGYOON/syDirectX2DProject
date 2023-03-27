#include "pch.h"
#include "PlayerAttacked.h"
#include "FSM.h"
#include "Animator.h"

PlayerAttacked::PlayerAttacked(Player* player, FSM* stateMachine, const wstring& stateName)
	:PlayerPassiveState(player, stateMachine, stateName)
{
}

void PlayerAttacked::Update()
{
	PlayerState::Update();
	CheckTransition();
}

void PlayerAttacked::Enter()
{
	PlayerPassiveState::Enter();
	_player->_invisible = true;
}

void PlayerAttacked::Exit()
{
	PlayerPassiveState::Exit();
	_player->_invisible = false;
}

void PlayerAttacked::CheckTransition()
{
	PlayerPassiveState::CheckTransition();

	if (isExitingState)
		return;

	if (elapsedTime >0.3f) {
		_fsm->ChangeState(_player->idle.get(), FSM::READY);
		_player->_attacked = false;
		isExitingState = true;
	}
}