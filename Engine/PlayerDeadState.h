#pragma once
#include "PlayerPassiveState.h"
class PlayerDeadState : public PlayerPassiveState
{
public:
	PlayerDeadState(Player* player, FSM* stateMachine, const wstring& stateName);
};

