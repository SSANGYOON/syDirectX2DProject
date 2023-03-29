#include "pch.h"
#include "PlayerAttackState.h"
#include "Weapon.h"
#include "Input.h"
#include "Animator.h"
#include "FSM.h"

PlayerAttackState::PlayerAttackState(Player* player, FSM* stateMachine, const wstring& stateName, const UINT8 slot) 
	: PlayerActiveState(player, stateMachine, stateName), weapon(nullptr), _slot(slot)
{

}

void PlayerAttackState::Update()
{
	PlayerActiveState::Update();
	if (isExitingState)
	{
		weapon->Stop();
		return;
	}

	CheckTransition();

}

void PlayerAttackState::Enter()
{
	PlayerActiveState::Enter();
	weapon = _player->GetWeapon(_slot);
	_stateName = weapon->GetWeaponAnimKey();
	if (!_player->_ground)
		_stateName += L"AERIAL";
	else if (INPUT->GetKeyState(KEY_TYPE::DOWN) == KEY_STATE::PRESS)
		_stateName += L"CROUCH";
	else
		_stateName += L"STANDING";
	weapon->SetUse();
}

void PlayerAttackState::Exit()
{

}

void PlayerAttackState::CheckTransition()
{
	PlayerActiveState::CheckTransition();
	if (elapsedTime > weapon->weaponDesc->duration)
		_fsm->ChangeState(_player->idle.get(), FSM::READY);
}
