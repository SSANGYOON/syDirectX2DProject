#include "pch.h"
#include "PlayerGuarded.h"

PlayerGuarded::PlayerGuarded(Player* player, FSM* stateMachine, const wstring& stateName)
	:PlayerPassiveState(player, stateMachine, stateName)
{
}
