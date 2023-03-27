#pragma once

class Script;
class FSM;
class ObjectState
{
public:
	
	ObjectState(FSM* fsm, const wstring& stateName);

	virtual void Enter() abstract;
	virtual void Update() abstract;
	virtual void Exit() abstract;
	
protected:
	FSM* _fsm;
	wstring _stateName;
	bool isExitingState;
};

