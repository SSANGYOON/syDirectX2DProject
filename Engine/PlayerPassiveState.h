#pragma once
#include "PlayerState.h"
class PlayerPassiveState : public PlayerState
{
public:
	PlayerPassiveState(Player* player, FSM* stateMachine, const wstring& stateName);

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CheckTransition() override;
};

