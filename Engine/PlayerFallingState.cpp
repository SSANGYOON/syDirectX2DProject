#include "pch.h"
#include "PlayerFallingState.h"
#include "FSM.h"
#include "Animator.h"

PlayerFallingState::PlayerFallingState(Player* player, FSM* stateMachine, const wstring& stateName)
	:PlayerNormalState(player, stateMachine, stateName)
{
}

void PlayerFallingState::Update()
{
	PlayerNormalState::Update();
	CheckTransition();
}

void PlayerFallingState::Enter()
{
	PlayerNormalState::Enter();
}

void PlayerFallingState::Exit()
{
	PlayerNormalState::Exit();
}

void PlayerFallingState::CheckTransition()
{
	PlayerNormalState::CheckTransition();

	if (isExitingState)
		return;

	if (_player->_ground) {
		_fsm->ChangeState(_player->idle.get(), FSM::YET);
		_player->GetAnimator()->Play(L"LANDING");

		isExitingState = true;
	}
}
