#pragma once
#include "Script.h"

class Collider;
class Animator;
class BaseRenderer;
class RigidBody;
class FSM;
class PlayerState;
class Weapon;
class Skill;

class Player : public Script
{
public:
    Player();
    virtual ~Player();

    virtual void Start() override;
    virtual void Update() override;
    virtual Animator* GetAnimator() { return animator; }

    virtual void OntriggerEnter(Collider* collider) override;
    virtual void OntriggerExit(Collider* collider) override;

    virtual void SetWeapon(UINT8 slot, Weapon* weapon);
    Weapon* GetWeapon(UINT8 slot) { return weapons[slot]; }
    virtual void SetSkill(UINT8 slot, shared_ptr<Skill> skill);

private:
    Animator* animator;
    RigidBody* rigidBody;
    Transform* transform;
    array<Weapon*, 2> weapons;
    array<shared_ptr<Skill>, 2> skills;

private:
    void Move();
    void Jump();

    void Ability();

    void Attacked();

    void Guarded();
    void Flip();

    void TransitionEnd();

private:
    const Vector3 jumpForce;
    const float moveSpeed;
    const float smoothTime;

    Vector3 accel;
    Vector3 velocity;

public:
    unique_ptr<FSM> playerFSM;
    unique_ptr<PlayerState> idle;
    unique_ptr<PlayerState> move;
    unique_ptr<PlayerState> jump;
    unique_ptr<PlayerState> fall;
    unique_ptr<PlayerState> crouch;
    unique_ptr<PlayerState> roll;
    unique_ptr<PlayerState> attacked;
    unique_ptr<PlayerState> guarded;
    unique_ptr<PlayerState> dead;

    unique_ptr<PlayerState> attack1;
    unique_ptr<PlayerState> attack2;
    unique_ptr<PlayerState> skill1;
    unique_ptr<PlayerState> skill2;

    bool _facingRight;
    bool _ground;
    bool _jump;
    bool _attacked;
    bool _falling;
    bool _invisible;

    const float evadeTime = 1.f;
    const float evadeSpeed = 15.f;
};

