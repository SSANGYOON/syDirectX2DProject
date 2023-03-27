#pragma once
#include "PlayerNormalState.h"

class PlayerIdleState : public PlayerNormalState
{
public:
	PlayerIdleState(Player* player, FSM* stateMachine, const wstring& stateName);

	virtual void Update() override;
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CheckTransition() override;
};

