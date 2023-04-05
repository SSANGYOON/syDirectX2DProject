#include "pch.h"
#include "Player.h"

#include "GameObject.h"

#include "Collider.h"
#include "RigidBody.h"
#include "Animator.h"
#include "SpriteRenderer.h"
#include "Collider2D.h"

#include "Resources.h"
#include "Mesh.h"
#include "Material.h"

#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"

#include "FSM.h"
#include "PlayerIdleState.h"
#include "PlayerMoveState.h"
#include "PlayerJumpState.h"
#include "PlayerFallingState.h"
#include "PlayerCrouchState.h"
#include "PlayerAttackState.h"
#include "PlayerRollState.h"
#include "PlayerAttacked.h"
#include "PlayerGuarded.h"
#include "PlayerDeadState.h"
#include "PlayerSummonState.h"

#include "Weapon.h"
#include "Skill.h"

Player::Player(GameObject* owner)
	: Script(owner), _facingRight(true), _ground(false), _jump(false), _attacked(false), _falling(false), _invisible(false)
	, jumpForce(Vector3(0.f, 30.f, 0.f)), moveSpeed(10.0f), velocity(Vector3::Zero), accel(Vector3::Zero) ,smoothTime(0.1f)
{
	transform = owner->GetTransform();
	animator = owner->AddComponent<Animator>();
	rigidBody = owner->AddComponent<RigidBody>();
	rigidBody->SetGravity(true);
	SpriteRenderer* sr = owner->AddComponent<SpriteRenderer>();

	shared_ptr<Material> material = make_shared<Material>();
	material->Load(L"Heroine_RenderPath.json");
	sr->SetMaterial(material);

	Animator* anim = owner->AddComponent<Animator>();

	collider = owner->AddComponent<Collider2D>();
	collider->SetType(Collider_TYPE::RECTANGLE);
	collider->SetSize(Vector3(1.f, 4.0f, 1.f));
	collider->SetLocalCenter(Vector3(0.f, -0.5f, 0.f));

	Collider2D* groundChecker = owner->AddComponent<Collider2D>();
	groundChecker->SetType(Collider_TYPE::CIRCLE);
	groundChecker->SetSize(Vector3(0.8f, 0.8f, 1.0));
	groundChecker->SetLocalCenter(Vector3(0.f, -2.5f, 0.f));
	groundChecker->SetTrigger(true);

	GameObject* WeaponObj = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::PLAYER_WEAPON);
	Weapon* weapon = WeaponObj->AddComponent<Weapon>();

	transform->SetChild(WeaponObj->GetTransform(), L"weapon1");
	weapons[0] = weapon;

	GameObject* WeaponObj2 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::PLAYER_WEAPON);
	Weapon* weapon2 = WeaponObj2->AddComponent<Weapon>();
	transform->SetChild(WeaponObj2->GetTransform(), L"weapon2");
	weapons[1] = weapon2;

	GameObject* SkillObj1 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::PLAYER_WEAPON);
	Skill* skill1 = SkillObj1->AddComponent<Skill>();
	transform->SetChild(SkillObj1->GetTransform(), L"skill1");
	skills[0] = skill1;
}

Player::~Player()
{
}

void Player::Start()
{

	animator->LoadAnimation2dFromJson("Heroine_clips.json");

    playerFSM = make_unique<FSM>();
	idle = make_unique<PlayerIdleState>(this,playerFSM.get(), L"IDLE");
	playerFSM->Initialize(idle.get());
	move = make_unique<PlayerMoveState>(this, playerFSM.get(), L"RUN");
	jump = make_unique<PlayerJumpState>(this, playerFSM.get(), L"JUMP");
	fall = make_unique<PlayerFallingState>(this, playerFSM.get(), L"FALLING");
	crouch = make_unique<PlayerCrouchState>(this, playerFSM.get(), L"CROUCHING");

	roll = make_unique<PlayerRollState>(this, playerFSM.get(), L"ROLLING");

	attack1 = make_unique<PlayerAttackState>(this, playerFSM.get(), L"attack1",0);
	attack2 = make_unique<PlayerAttackState>(this, playerFSM.get(), L"attack2",1);
	skill1 = make_unique<PlayerSummonState>(this, playerFSM.get(), L"STANDSUMMON", 0);
	//skill2 = make_unique<PlayerAttackState>(this, playerFSM.get(), L"skill2");


	attacked = make_unique<PlayerAttacked>(this, playerFSM.get(), L"ATTACKED");
	guarded = make_unique<PlayerGuarded>(this, playerFSM.get(), L"GUARDED");
	dead = make_unique<PlayerGuarded>(this, playerFSM.get(), L"DEAD");

	weapons[0] = transform->GetChild(L"weapon1")->GetOwner()->GetComponent<Weapon>();
	weapons[0]->SetWeaponType(WEAPON_TYPE::ONEHAND);
	weapons[1] = transform->GetChild(L"weapon2")->GetOwner()->GetComponent<Weapon>();
	weapons[1]->SetWeaponType(WEAPON_TYPE::DAGGER);
	transform->SetPosition(Vector3::Up * 10.f);
}
void Player::Update()
{
	if (velocity.y < 0.f)
		_falling = true;
	else
		_falling = false;

	Move();
	Jump();
	Ability();

	playerFSM->Update();
}

void Player::OntriggerEnter(Collider* collider)
{
	GameObject* opponent = collider->GetOwner();
	Transform* opponentTr = opponent->GetTransform();

	LAYER_TYPE type = opponent->GetType();

	switch (type)
	{

	case LAYER_TYPE::FIXEDOBJECT:
		_jump = true;
		_ground = true;
		break;
	case LAYER_TYPE::MONSTER:
	{
		PlayerState* ps = static_cast<PlayerState*>(playerFSM->GetCurrentState());
		if (_invisible)
			return;

		if (!ps->_guard) {
			if (_facingRight) {
				if (opponentTr->GetWorldPosition().x - transform->GetWorldPosition().x < 0.f)
					_attacked = true;
			}
			else {
				if (opponentTr->GetWorldPosition().x - transform->GetWorldPosition().x > 0.f)
					_attacked = true;
			}
		}
		else
			_attacked = true;
		if (_attacked)
			Attacked();
	}
	break;
	default:
		break;
	}
		
}


void Player::OntriggerExit(Collider* collider)
{
	GameObject* opponent = collider->GetOwner();
	LAYER_TYPE type = opponent->GetType();
	switch (type)
	{
	case LAYER_TYPE::FIXEDOBJECT:
		_ground = false;
		break;
	default:
		break;
	}
}

void Player::SetWeapon(UINT8 slot, Weapon* weapon)
{
	assert(slot < 2);
	weapons[slot] = weapon;
}

void Player::SetSkill(UINT8 slot, Skill* skill)
{
	assert(slot < 2);
	skills[slot] = skill;
}

void Player::Move()
{
	if (_attacked)
		return;

	auto p = static_cast<PlayerState*>(playerFSM->GetCurrentState());
	velocity = rigidBody->GetVelocity();
	if (playerFSM->GetCurrentState() == roll.get())
	{
		float delta = p->GetTime() / evadeTime;
		float targetSpeed = (1.f - delta) * (evadeSpeed - moveSpeed / 2.f) + delta * moveSpeed / 2.f;
		if (_facingRight)
			rigidBody->SetVelocity({ targetSpeed, velocity.y, velocity.x });
		else
			rigidBody->SetVelocity({ -targetSpeed, velocity.y, velocity.x });
	}
	else if (p->_controllable)
	{
		if (INPUT->GetKeyState(KEY_TYPE::RIGHT) == KEY_STATE::PRESS)
			rigidBody->SetVelocity(RigidBody::SmoothDamp(velocity, Vector3(moveSpeed, velocity.y, velocity.z), accel, smoothTime));

		else if (INPUT->GetKeyState(KEY_TYPE::LEFT) == KEY_STATE::PRESS)
			rigidBody->SetVelocity(RigidBody::SmoothDamp(velocity, Vector3(-moveSpeed, velocity.y, velocity.z), accel, smoothTime));

		else
			rigidBody->SetVelocity(RigidBody::SmoothDamp(velocity, Vector3(0.f, velocity.y, velocity.z), accel, smoothTime));
	}
	else 
		rigidBody->SetVelocity(RigidBody::SmoothDamp(velocity, Vector3(0.f, velocity.y, velocity.z), accel, smoothTime));

	if (_facingRight && velocity.x < 0.f || !_facingRight && velocity.x > 0.f)
		Flip();
}

void Player::Flip()
{
	_facingRight = !_facingRight;

	Vector3 theScale = transform->GetScale();
	theScale.x *= -1.f;
	transform->SetScale(theScale);
}

void Player::Jump()
{
	auto p = static_cast<PlayerState*>(playerFSM->GetCurrentState());
	if (p->_controllable && _jump)
	{
		if (GET_SINGLE(Input)->GetKeyState(KEY_TYPE::SPACE) == KEY_STATE::DOWN)
		{
			rigidBody->ApplyForce(jumpForce, RigidBody::IMPULSE);
			_jump = false;
		}
	}
}

void Player::Ability()
{
}

void Player::Attacked()
{
	if (_facingRight)
		velocity = Vector3(-7.f, 7.f, 0.f);
	else
		velocity = Vector3(7.f, 7.f, 0);
	rigidBody->SetVelocity(velocity);
}

void Player::Guarded()
{
}

void Player::TransitionEnd()
{
	playerFSM->SetTransitionState(FSM::READY);
}