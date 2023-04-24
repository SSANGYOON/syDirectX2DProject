using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics.Eventing.Reader;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using SY;

namespace Sandbox
{
    public class Player : Entity
    {
        public enum PlayerState
        { 
            Idle,
            Run,
            Aerial,
            Crouching,
            Roll,
            Attack,
            Casting,
            Damaged,
            Dead
        }

        private TransformComponent m_Transform;
        private Rigidbody2DComponent m_Rigidbody;
        private SpriteAnimatorComponent m_Animator;
        private PlayerState m_State;

        public float Speed = 10.0f;
        public float Time = 0.0f;

        private Weapon weapon = null;
        private Skill skill = null;

        private bool isGrounded = false;
        private bool isFalling = false;
        public PlayerState State
        {
            get { return m_State; }
            set
            {
                PlayerState prevState = m_State;
                m_State = value;

                switch (m_State)
                {
                    case PlayerState.Idle:
                        if (prevState == PlayerState.Aerial)
                            m_Animator.Play("Landing");
                        else if (prevState == PlayerState.Run)
                            m_Animator.Play("Run2Idle");
                        else if (prevState == PlayerState.Crouching)
                            m_Animator.Play("Crouching2Idle");
                        else
                            m_Animator.Play("IDLE");
                        break;

                    case PlayerState.Run:
                        if (prevState == PlayerState.Idle)
                            m_Animator.Play("Idle2Run");
                        else
                            m_Animator.Play("Run");
                        break;

                    case PlayerState.Aerial:
                        if (isFalling)
                            m_Animator.Play("Falling");
                        else
                            m_Animator.Play("Jump");
                        break;

                    case PlayerState.Crouching:
                        m_Animator.Play("Crouch");
                        break;

                    case PlayerState.Roll:
                        m_Animator.Play("Roll");
                        break;

                    case PlayerState.Attack:
                        if (weapon == null)
                            m_State = prevState;
                        else {
                            string Pose;
                            if (isGrounded && Input.IsKeyDown(KeyCode.Down))
                                Pose = "Crouching";
                            else if (isGrounded)
                                Pose = "Standing";
                            else
                                Pose = "Aerial";

                            m_Animator.Play("Attack" + Enum.GetName(typeof(Weapon.WeaponType), weapon.weaponType) + Pose);
                        }
                        break;

                    case PlayerState.Casting:
                        if (weapon == null)
                            m_State = prevState;
                        else
                        {
                            string Pose;
                            if (isGrounded && Input.IsKeyDown(KeyCode.Down))
                                Pose = "Crouching";
                            else if (isGrounded)
                                Pose = "Standing";
                            else
                                Pose = "Aerial";
                            m_Animator.Play("Casting" + skill.skillType.ToString() + Pose);
                        }
                        break;

                    case PlayerState.Damaged:
                        m_Animator.Play("Damaged");
                        break;
                    case PlayerState.Dead:
                        m_Animator.Play("Dead");
                        break;
                    default:
                        break;
                }
            }
        }

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();
            m_Animator = GetComponent<SpriteAnimatorComponent>();
            m_State = PlayerState.Idle;
        }

        void OnUpdate(float ts)
        {

            UpdateState();

            if (m_State == PlayerState.Idle || m_State == PlayerState.Run || m_State == PlayerState.Aerial)
            {
                Vector2 velocity = Vector2.Zero;
                if(Input.IsKeyDown(KeyCode.Right))
                    velocity.X += 1;
                if(Input.IsKeyDown(KeyCode.Left))
                    velocity.X -= 1;

                m_Rigidbody.SetLinearVelocity(velocity * Speed, true);
            }
        }

        private void UpdateState()
        {
            switch (State) {
                case PlayerState.Idle:
                case PlayerState.Run:
                case PlayerState.Aerial:
                case PlayerState.Crouching:
                    if (isGrounded == false)
                        if (State != PlayerState.Aerial)
                            State = PlayerState.Aerial;
                    else if (State == PlayerState.Idle && Math.Abs(m_Rigidbody.LinearVelocity.X) >= 1.0f)
                        State = PlayerState.Run;
                    else if (State == PlayerState.Run && Math.Abs(m_Rigidbody.LinearVelocity.X) < 1.0f)
                        State = PlayerState.Idle;
                    else if (Input.IsKeyDown(KeyCode.Down))
                        if(State != PlayerState.Crouching)
                            State = PlayerState.Crouching;
                    if (Input.IsKeyDown(KeyCode.Z))
                        State = PlayerState.Attack;
                    if (Input.IsKeyDown(KeyCode.A))
                        State = PlayerState.Casting;
                    break;
                default: break;
            }
        }
        void OnTriggerEnter(ref Collision2D collsion) 
        { }
        void OnTriggerStay(ref Collision2D collision)
        { }
        void OnTriggerExit(ref Collision2D collision) 
        { }

        void OnCollisionStay(ref Collision2D collision)
        {
            Console.WriteLine($"Collsion {collision.CollisionLayer}");
        }

    }
}