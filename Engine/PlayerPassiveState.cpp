#include "pch.h"
#include "PlayerPassiveState.h"
#include "Player.h"
#include "FSM.h"
#include "Animator.h"

PlayerPassiveState::PlayerPassiveState(Player* player, FSM* stateMachine, const wstring& animBoolName)
	:PlayerState(player, stateMachine, animBoolName)
{
}

void PlayerPassiveState::Enter()
{
	PlayerState::Enter();
}

void PlayerPassiveState::Exit()
{
}


void PlayerPassiveState::CheckTransition()
{
}