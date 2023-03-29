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
#include "Shader.h"
#include "Texture.h"

#include "Input.h"
#include "Timer.h"

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

#include "Weapon.h"
#include "WeaponManager.h"

#include "PlayerSummonState.h"
#include "Skill.h"

Player::Player(GameObject* owner)
	: Script(owner), _facingRight(true), _ground(false), _jump(false), _attacked(false), _falling(false), _invisible(false)
	, jumpForce(Vector3(0.f, 20.f, 0.f)), moveSpeed(10.0f), velocity(Vector3::Zero), accel(Vector3::Zero) ,smoothTime(0.1f)
{
	transform = owner->GetTransform();
	animator = owner->AddComponent<Animator>();
	rigidBody = owner->AddComponent<RigidBody>();
	rigidBody->SetGravity(true);
	SpriteRenderer* sr = owner->AddComponent<SpriteRenderer>();

	shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
	shared_ptr<Material> material = make_shared<Material>();
	material->Load(L"Heroine_RenderPath.json");
	sr->SetMaterial(material);
	sr->SetMesh(mesh);
	Animator* anim = owner->AddComponent<Animator>();

	Collider2D* col = owner->AddComponent<Collider2D>();
	col->SetType(Collider_TYPE::RECTANGLE);
	col->SetSize(Vector3(1.f, 4.2f, 1.f));
	col->SetLocalCenter(Vector3(0.f, -0.5f, 0.f));

	Collider2D* col2 = owner->AddComponent<Collider2D>();
	col2->SetType(Collider_TYPE::CIRCLE);
	col2->SetSize(Vector3(0.2f, 0.2f, 1.0));
	col2->SetLocalCenter(Vector3(0.f, -2.5f, 0.f));
	col2->SetTrigger(true);
}

Player::~Player()
{
}

void Player::Start()
{

	animator->LoadAnimation2dFromJson("c:\\Users\\eondr\\source\\repos\\syDirectX2DProject\\Resources\\Heroine_clips.json");


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

	GET_SINGLE(WeaponManager)->Init();

	attacked = make_unique<PlayerAttacked>(this, playerFSM.get(), L"ATTACKED");
	guarded = make_unique<PlayerGuarded>(this, playerFSM.get(), L"GUARDED");
	dead = make_unique<PlayerGuarded>(this, playerFSM.get(), L"DEAD");


	animator->GetClip(L"IDLE2RUN")->addEvent(0.2f, bind(&Player::TransitionEnd, this));
	animator->GetClip(L"RUN2IDLE")->addEvent(0.2f, bind(&Player::TransitionEnd, this));
	animator->GetClip(L"LANDING")->addEvent(0.2f, bind(&Player::TransitionEnd, this));
	animator->GetClip(L"CROUCH")->addEvent(0.2f, bind(&Player::TransitionEnd, this));
	animator->GetClip(L"CROUCH2IDLE")->addEvent(0.2f, bind(&Player::TransitionEnd, this));
	animator->GetClip(L"STANDCASTINGEND")->addEvent(0.3f, bind(&Player::TransitionEnd, this));

	//weapons[0] = transform->GetChild(L"weapon1")->GetOwner()->GetComponent<Weapon>();
	//weapons[0]->SetWeaponType(WEAPON_TYPE::ONEHAND);
	//weapons[1] = transform->GetChild(L"weapon2")->GetOwner()->GetComponent<Weapon>();

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