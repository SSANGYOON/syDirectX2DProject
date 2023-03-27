#include "pch.h"
#include "PlayerState.h"
#include "Player.h"
#include "FSM.h"
#include "Animator.h"
#include "Timer.h"
PlayerState::PlayerState(Player* player, FSM* fsm, const wstring& stateName)
	:ObjectState(fsm, stateName), _player(player), _controllable(false),  _invisible(false), _guard(true)
{

}

void PlayerState::Update()
{
	if (_fsm->GetTransitionState() == FSM::READY) {
		_player->GetAnimator()->Play(_stateName);
		_fsm->SetTransitionState(FSM::DONE);
	}
	elapsedTime += TIME->DeltaTime();
}

void PlayerState::Enter()
{
	elapsedTime = 0.f;
	isExitingState = false;
}
