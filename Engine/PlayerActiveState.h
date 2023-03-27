#pragma once
#include "PlayerState.h"
class PlayerActiveState : public PlayerState
{
public:
	PlayerActiveState(Player* player, FSM* stateMachine, const wstring& stateName);

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CheckTransition() override;
};

