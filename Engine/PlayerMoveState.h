#pragma once
#include "PlayerNormalState.h"
class PlayerMoveState : public PlayerNormalState
{
public:
	PlayerMoveState(Player* player, FSM* stateMachine, const wstring& stateName);

	virtual void Update() override;
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CheckTransition() override;
};

