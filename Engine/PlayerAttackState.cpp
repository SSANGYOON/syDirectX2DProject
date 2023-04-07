#include "pch.h"
#include "PlayerAttackState.h"
#include "Weapon.h"
#include "Input.h"
#include "Animator.h"
#include "FSM.h"
#include "Collider.h"

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
	{
		_stateName += L"CROUCHING";
		_player->collider->SetSize(Vector3(10.f, 25.f, 1.f));
		_player->collider->SetLocalCenter(Vector3(0.f, -5.f, 0.f));
	}
	else
		_stateName += L"STANDING";
	weapon->SetUse();
}

void PlayerAttackState::Exit()
{
	_player->collider->SetSize(Vector3(10.f, 40.f, 1.f));
	_player->collider->SetLocalCenter(Vector3(0.f, -5.f, 0.f));
}

void PlayerAttackState::CheckTransition()
{
	PlayerActiveState::CheckTransition();
	if (!weapon->IsUsing()) {
		if (INPUT->GetKeyState(KEY_TYPE::DOWN) == KEY_STATE::PRESS)
			_fsm->ChangeState(_player->crouch.get(), FSM::READY);
		else
			_fsm->ChangeState(_player->idle.get(), FSM::READY);
	}
}
