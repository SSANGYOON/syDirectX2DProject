#pragma once
#include "PlayerState.h"

class PlayerNormalState : public PlayerState
{
public :
	PlayerNormalState(Player* player, FSM* stateMachine, const wstring& stateName);

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CheckTransition() override;
};

