#include "pch.h"
#include "ObjectState.h"
#include "Script.h"
#include "FSM.h"


ObjectState::ObjectState(FSM* fsm, const wstring& stateName)
    :_fsm(fsm), _stateName(stateName), isExitingState(false)
{
}
