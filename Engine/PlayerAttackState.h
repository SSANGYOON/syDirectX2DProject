#pragma once
#include "PlayerActiveState.h"
class Weapon;
class PlayerAttackState : public PlayerActiveState
{
public:
	PlayerAttackState(Player* player, FSM* stateMachine, const wstring& stateName, const UINT8 slot);

	virtual void Update() override;
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CheckTransition() override;

private:
	UINT8 _slot;
	Weapon* weapon;
};

