#pragma once
#include "ObjectState.h"
#include "Player.h"


class PlayerState : public ObjectState
{
public:
	PlayerState(Player* player, FSM* fsm, const wstring& stateName);

	virtual void Update() override;
	virtual void Enter() override;
	virtual void CheckTransition() abstract;

public:
	float GetTime() { return elapsedTime;}

protected:
	friend class Player;
	Player* _player;

	bool _controllable;
	bool _invisible;
	bool _guard;
	float elapsedTime;
};

