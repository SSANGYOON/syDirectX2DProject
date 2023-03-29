#pragma once
#include "PlayerActiveState.h"
class PlayerSummonState : public PlayerActiveState
{
public:
	PlayerSummonState(Player* player, FSM* stateMachine, const wstring& stateName, const UINT8 slot);

	virtual void Update() override;
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CheckTransition() override;

private:
	UINT8 _slot;
	class Skill* skill;
};

