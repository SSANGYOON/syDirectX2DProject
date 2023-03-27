#include "pch.h"
#include "PlayerJumpState.h"
#include "FSM.h"
#include "Animator.h"

PlayerJumpState::PlayerJumpState(Player* player, FSM* stateMachine, const wstring& stateName)
	:PlayerNormalState(player, stateMachine, stateName)
{
}

void PlayerJumpState::Update()
{
	PlayerState::Update();
	CheckTransition();
}

void PlayerJumpState::Enter()
{
	PlayerNormalState::Enter();
}

void PlayerJumpState::Exit()
{
	PlayerNormalState::Exit();
}

void PlayerJumpState::CheckTransition()
{
	PlayerNormalState::CheckTransition();

	if (isExitingState)
		return;
	
	if (_player->_falling) {
		_fsm->ChangeState(_player->fall.get(), FSM::READY);
		isExitingState = true;
	}
}

