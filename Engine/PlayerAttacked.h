#pragma once
#include "PlayerPassiveState.h"

class PlayerAttacked : public PlayerPassiveState
{
public:
	PlayerAttacked(Player* player, FSM* stateMachine, const wstring& stateName);

	virtual void Update() override;
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CheckTransition() override;
};

