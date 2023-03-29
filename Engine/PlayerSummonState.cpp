#include "pch.h"
#include "PlayerSummonState.h"
#include "FSM.h"
#include "Skill.h"
#include "Animator.h"

PlayerSummonState::PlayerSummonState(Player* player, FSM* stateMachine, const wstring& stateName, const UINT8 slot)
	: PlayerActiveState(player, stateMachine, stateName), skill(nullptr), _slot(slot)
{
}

void PlayerSummonState::Update()
{
	PlayerActiveState::Update();
	if (!isExitingState)
		CheckTransition();
}

void PlayerSummonState::Enter()
{
	PlayerActiveState::Enter();
	//skill = _player->GetWeapon(_slot);
	//_stateName = weapon->GetWeaponAnimKey();
	//if (!_player->_ground)
	//	_stateName += L"AERIAL";
	//else if (INPUT->GetKeyState(KEY_TYPE::DOWN) == KEY_STATE::PRESS)
	//	_stateName += L"CROUCH";
	//else
	//	_stateName += L"STANDING";
	//weapon->SetUse();
}

void PlayerSummonState::Exit()
{
}

void PlayerSummonState::CheckTransition()
{
	if (elapsedTime > 1.25f) {
		_fsm->ChangeState(_player->idle.get(), FSM::YET);
		_player->GetAnimator()->Play(L"STANDCASTINGEND");
		isExitingState = true;
	}
}
