#include "pch.h"
#include "PlayerDeadState.h"

PlayerDeadState::PlayerDeadState(Player* player, FSM* stateMachine, const wstring& stateName)
	:PlayerPassiveState(player, stateMachine, stateName)
{
}
