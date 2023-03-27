#pragma once
#include "Script.h"

class ObjectState;
class FSM
{
public:

	void Initialize(ObjectState* startingState);

	enum TransitionState
	{
		YET,
		READY,
		DONE
	};

	void ChangeState(ObjectState* newState, TransitionState tstate);

	void Update();

	void SetTransitionState(TransitionState state) { _tstate = state; }
	TransitionState GetTransitionState() { return _tstate; }

	ObjectState* GetCurrentState() { return CurrentState; }

private:
	ObjectState* CurrentState;
	TransitionState _tstate;
};

