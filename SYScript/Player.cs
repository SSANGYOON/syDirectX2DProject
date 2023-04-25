using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics.Eventing.Reader;
using System.Linq;
using System.Reflection;
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
        private BoxCollider2DComponent m_BoxCollider;
        private PlayerState m_State;

        public float Speed = 10.0f;
        public float stateTime = 0.0f;

        private Weapon weapon = null;
        private Skill skill = null;

        private bool isGrounded = false;
        private bool isFalling = false;
        private bool flip = false;
        private bool invisible = false;
        private const float evadeTime = 1.0f;
        private const float evadeSpeed = 150.0f;
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
                        {
                            Vector2 size = m_BoxCollider.Size;
                            size.Y = 24.0f;
                            m_BoxCollider.Size = size;
                            m_Animator.Play("Crouching2Idle");
                        }
                        else if (prevState != PlayerState.Idle)
                            m_Animator.Play("Idle");
                        break;

                    case PlayerState.Run:
                        if (prevState == PlayerState.Idle)
                            m_Animator.Play("Idle2Run");
                        else if(prevState != PlayerState.Run)
                            m_Animator.Play("Run");
                        break;

                    case PlayerState.Aerial:
                        if (isFalling)
                            m_Animator.Play("Falling");
                        else
                            m_Animator.Play("Jump");
                        break;

                    case PlayerState.Crouching:
                        {
                            m_Animator.Play("Crouch");
                            Vector2 size = m_BoxCollider.Size;
                            size.Y = 19.0f;
                            m_BoxCollider.Size = size;
                        }
                        break;

                    case PlayerState.Roll:
                        m_Animator.Play("Roll");
                        break;

                    case PlayerState.Attack:
                        if (weapon == null)
                            return;
                        else
                        {
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
                            return;
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
                stateTime = 0;
            }
        }

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();
            m_Animator = GetComponent<SpriteAnimatorComponent>();
            m_BoxCollider = GetComponent<BoxCollider2DComponent>();
            m_State = PlayerState.Idle;
        }

        void OnUpdate(float ts)
        {
            stateTime += ts;
            UpdateState();
            if (m_State == PlayerState.Idle || m_State == PlayerState.Run || m_State == PlayerState.Aerial)
            {
                Vector2 velocity = m_Rigidbody.LinearVelocity;
                velocity.X = 0;
                if (Input.IsKeyDown(KeyCode.Right))
                    velocity.X += 1 * Speed;
                if (Input.IsKeyDown(KeyCode.Left))
                    velocity.X -= 1 * Speed;

                m_Rigidbody.LinearVelocity = velocity;

                if (Input.IsKeyDown(KeyCode.Space) && (m_State == PlayerState.Idle || m_State == PlayerState.Run))
                    m_Rigidbody.ApplyLinearImpulse(new Vector2(0, 1000 * m_BoxCollider.Size.Y * m_BoxCollider.Size.X), true);
                if (velocity.X < 0 && !flip || velocity.X > 0 && flip)
                {
                    Vector3 scale = Scale;
                    scale.X = -scale.X;
                    Scale = scale;
                    flip = !flip;
                }
            }
            if (m_State == PlayerState.Roll)
            {
                float delta = stateTime / evadeTime;
                float targetSpeed = (1.0f - delta) * (evadeSpeed) + delta * Speed / 2.0f;
                if (flip)
                    m_Rigidbody.LinearVelocity = new Vector2(-targetSpeed, m_Rigidbody.LinearVelocity.Y);
                else
                    m_Rigidbody.LinearVelocity = new Vector2(targetSpeed, m_Rigidbody.LinearVelocity.Y);
            }
        }

        private void UpdateState()
        {
            if (m_Rigidbody.LinearVelocity.Y < 0)
            {
                if (isFalling == false && State == PlayerState.Aerial)
                    m_Animator.Play("Falling");
                isFalling = true;
            }
            else
            {
                isFalling = false;
            }
                
            switch (State) {
                case PlayerState.Idle:
                case PlayerState.Run:
                case PlayerState.Aerial:
                case PlayerState.Crouching:
                    if (isGrounded == false)
                    {
                        if (State != PlayerState.Aerial)
                            State = PlayerState.Aerial; 
                    }
                    else {
                        if (Input.IsKeyDown(KeyCode.Down) == false && Math.Abs(m_Rigidbody.LinearVelocity.X) < 1.0f)
                            State = PlayerState.Idle;
                        else if (State == PlayerState.Idle && Math.Abs(m_Rigidbody.LinearVelocity.X) >= 1.0f)
                            State = PlayerState.Run;
                        else if (Input.IsKeyDown(KeyCode.Down))
                        {
                            if (State != PlayerState.Crouching)
                                State = PlayerState.Crouching;
                        }

                        if (Input.IsKeyDown(KeyCode.C) && m_State != PlayerState.Crouching)
                            State = PlayerState.Roll;
                    
                    }
                              
                    if (Input.IsKeyDown(KeyCode.Z))
                        State = PlayerState.Attack;
                    if (Input.IsKeyDown(KeyCode.A))
                        State = PlayerState.Casting;
                    break;
                case PlayerState.Roll:
                    if (stateTime > evadeTime)
                        State = PlayerState.Idle;
                    break;
                default: break;
            }
        }
        void OnCollisionStay(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 0)) > 0)
            {
                Entity ground = new Entity(collsion.EntityID);
                
                if (ground.Translation.Y < Translation.Y)
                    isGrounded = true;    
            }
        }

        void OnCollisionExit(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 0)) > 0)
            {
                Entity ground = new Entity(collsion.EntityID);

                if (ground.Translation.Y < Translation.Y)
                    isGrounded = false;
            }
        }

        public void OnNamedEvent(string funcName)
        {
            String name = funcName;
            Type type = this.GetType();
            MethodInfo myClass_FunCallme = type.GetMethod(name, BindingFlags.Instance | BindingFlags.NonPublic);
            myClass_FunCallme.Invoke(this, null);
        }

        void SetInvisible()
        {
            invisible = true;
        }
        void SetVulnerable()
        {
            invisible = false;
        }
    }
}