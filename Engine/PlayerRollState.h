#pragma once
#include "PlayerActiveState.h"
class PlayerRollState : public PlayerActiveState
{
public:
	PlayerRollState(Player* player, FSM* stateMachine, const wstring& stateName);

	virtual void Update() override;
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CheckTransition() override;
};

