using System;
using System.Reflection;

using SY;
using SYScript;

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
            ZAttack,
            XAttack,
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

        public Weapon zWeapon = null;
        public Weapon xWeapon = null;
        private Entity m_Inven = null;

        public Skill aSkill = null;
        public Skill sSkill = null;

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
                            size.Y = 15.0f;
                            m_BoxCollider.Size = size;
                        }
                        break;

                    case PlayerState.Roll:
                        m_Animator.Play("Roll");
                        break;

                    case PlayerState.ZAttack:
                    case PlayerState.XAttack:
                        if (m_State == PlayerState.ZAttack && zWeapon == null || m_State == PlayerState.XAttack && xWeapon == null)
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

                            if (m_State == PlayerState.ZAttack)
                            {
                                m_Animator.Play("Attack" + Enum.GetName(typeof(WeaponData.weaponType), zWeapon.Data.Type) + Pose);
                                zWeapon.Activate();
                                zWeapon.Attack();
                            }
                            else
                            {
                                m_Animator.Play("Attack" + Enum.GetName(typeof(WeaponData.weaponType), xWeapon.Data.Type) + Pose);
                                xWeapon.Activate();
                                xWeapon.Attack();
                            }
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
            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();
            m_Animator = GetComponent<SpriteAnimatorComponent>();
            m_BoxCollider = GetComponent<BoxCollider2DComponent>();
            m_State = PlayerState.Idle;

            Vector3 pos = new Vector3(0, 20, -1);
            DontDestroy(ref pos);

            m_Inven = FindEntityByName("InventoryPanel");
        }

        void OnUpdate(float ts)
        {

            if (State != PlayerState.Dead && Input.IsKeyDown(KeyCode.Tab))
            {
        
                if (m_Inven != null)
                {
                    m_Inven.GetComponent<StateComponent>().State = m_Inven.GetComponent<StateComponent>().State == StateComponent.EntityState.Pause ? 
                        StateComponent.EntityState.Active : StateComponent.EntityState.Pause;
                }
            }

            if (m_Inven != null)
            {
                if (m_Inven.IsValid())
                {
                    if (m_Inven.GetComponent<StateComponent>().State == StateComponent.EntityState.Active)
                        return;
                }
            }

            if (xWeapon == null)
                xWeapon = FindEntityByName("xWeapon").As<Weapon>();
            if(zWeapon == null)
                zWeapon = FindEntityByName("zWeapon").As<Weapon>();


            stateTime += ts;
            UpdateState();
            if (m_State == PlayerState.Idle || m_State == PlayerState.Run || m_State == PlayerState.Aerial)
            {
                Vector2 velocity = m_Rigidbody.LinearVelocity;
                velocity.X = 0;
                if (Input.IsKeyPressed(KeyCode.Right))
                    velocity.X += 1 * Speed;
                if (Input.IsKeyPressed(KeyCode.Left))
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
                        if (Input.IsKeyPressed(KeyCode.Down) == false && Math.Abs(m_Rigidbody.LinearVelocity.X) < 1.0f)
                            State = PlayerState.Idle;
                        else if (State == PlayerState.Idle && Math.Abs(m_Rigidbody.LinearVelocity.X) >= 1.0f)
                            State = PlayerState.Run;
                        else if (Input.IsKeyPressed(KeyCode.Down))
                        {
                            if (State != PlayerState.Crouching)
                                State = PlayerState.Crouching;
                        }

                        if (Input.IsKeyDown(KeyCode.C) && m_State != PlayerState.Crouching)
                            State = PlayerState.Roll;
                    
                    }
                              
                    if (Input.IsKeyDown(KeyCode.Z) && zWeapon.Data != null)
                        State = PlayerState.ZAttack;
                    if (Input.IsKeyDown(KeyCode.X) && xWeapon.Data != null)
                        State = PlayerState.XAttack;
                    break;
                case PlayerState.Roll:
                    if (stateTime > evadeTime)
                        State = PlayerState.Idle;
                    break;

                case PlayerState.ZAttack:
                case PlayerState.XAttack:
                    if (State == PlayerState.ZAttack && stateTime > zWeapon.Duration ||
                        State == PlayerState.XAttack && stateTime > xWeapon.Duration)
                    {
                        if(State == PlayerState.ZAttack)
                            zWeapon.Pause();
                        else
                            xWeapon.Pause();
                        if (isGrounded)
                        {
                            if (Input.IsKeyPressed(KeyCode.Down))
                                State = PlayerState.Crouching;
                            else
                                State = PlayerState.Idle;
                        }
                        else
                            State = PlayerState.Aerial;
                    }
                    break;

                default: break;
            }
        }
        void OnCollisionStay(ref Collision2D collsion)
        {
            Console.WriteLine("cOLLI");
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
                if (ground.IsValid())
                {
                    if (ground.Translation.Y < Translation.Y)
                        isGrounded = false;
                }
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