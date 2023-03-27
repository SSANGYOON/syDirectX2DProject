#pragma once
#include "PlayerPassiveState.h"
class PlayerGuarded : public PlayerPassiveState
{
public:
	PlayerGuarded(Player* player, FSM* stateMachine, const wstring& stateName);
};

