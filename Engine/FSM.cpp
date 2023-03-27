#include "pch.h"
#include "FSM.h"
#include "ObjectState.h"

void FSM::Initialize(ObjectState* startingState)
{
	_tstate = DONE;
	CurrentState = startingState;
	CurrentState->Enter();
}

void FSM::ChangeState(ObjectState* newState, TransitionState tstate)
{
	CurrentState->Exit();
	CurrentState = newState;
	newState->Enter();
	_tstate = tstate;
}

void FSM::Update()
{
	CurrentState->Update();
}
