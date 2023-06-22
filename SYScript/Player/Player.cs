using System;
using System.Reflection;

using SY;
using SYScript;

namespace Sandbox
{
    public class Player : Character
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
            XGuard,
            XGuardBreak,
            ZGuard,
            ZGuardBreak,
            Damaged,
            Dead
        }

        private SpriteAnimatorComponent m_Animator;
        private AudioSource m_AudioSource;

        private PlayerState m_State;

        public float Speed = 10.0f;

        public PlayerWeapon zWeapon = null;
        public PlayerWeapon xWeapon = null;
        private Entity m_Inven = null;

        public Skill aSkill = null;
        public Skill sSkill = null;

        private bool isGrounded = false;

        public bool Grounded
        {
            get { return isGrounded; }
        }
        private bool isFalling = false;
        
        private const float evadeTime = 1.0f;
        private const float evadeSpeed = 150.0f;
        public PlayerState State
        {
            get { return m_State; }
            set
            {
                PlayerState prevState = m_State;
                

                if (prevState == PlayerState.XAttack)
                    xWeapon.Pause();
                if (prevState == PlayerState.ZAttack)
                    zWeapon.Pause();

                switch (prevState)
                {
                    case PlayerState.Run:
                    case PlayerState.Roll:
                        m_AudioSource.Stop();
                        break;

                }
                
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
                            Vector2 size = hitBox.Size;
                            size.Y = 24.0f;
                            hitBox.Size = size;
                            m_Animator.Play("Crouching2Idle");
                        }
                        else if (prevState != PlayerState.Idle)
                            m_Animator.Play("Idle");
                        break;

                    case PlayerState.Run:
                        if (prevState == PlayerState.Idle)
                            m_Animator.Play("Idle2Run");

                        else
                            m_Animator.Play("Run");

                        m_AudioSource.Play("assets\\soundClip\\FOOTSTEP_Trainers_Gravel_Compact_Run_RR2_mono.wav", true);

                        break;

                    case PlayerState.Aerial:
                        if (isFalling)
                            m_Animator.Play("Falling");
                        else
                            m_Animator.Play("Jump");
                        break;

                    case PlayerState.Crouching:
                        if (prevState == PlayerState.Idle)
                            m_Animator.Play("Crouch");
                        else
                            m_Animator.Play("Crouching");
                        break;

                    case PlayerState.Roll:
                        m_Animator.Play("Roll");
                        m_AudioSource.Play("assets\\soundClip\\PlayerJumpRoll0.wav");
                        break;

                    case PlayerState.ZAttack:
                    case PlayerState.XAttack:
                        rigidBody.LinearVelocity = Vector2.Zero;
                        m_AudioSource.Play("assets\\soundClip\\PlayerAttackShort0.wav", false);
                        if (m_State == PlayerState.ZAttack && zWeapon == null || m_State == PlayerState.XAttack && xWeapon == null)
                            return;
                        else
                        {
                            string Pose;
                            if (isGrounded && Input.IsKeyPressed(KeyCode.Down))
                                Pose = "Crouching";
                            else if (isGrounded)
                                Pose = "Standing";
                            else
                                Pose = "Aerial";

                            if (m_State == PlayerState.ZAttack)
                            {
                                m_Animator.Play("Attack" + Enum.GetName(typeof(PlayerWeaponData.weaponType), zWeapon.Data.Type) + Pose);
                                zWeapon.Activate();
                                zWeapon.Active = true;
                                zWeapon.Attack();
                            }
                            else
                            {
                                m_Animator.Play("Attack" + Enum.GetName(typeof(PlayerWeaponData.weaponType), xWeapon.Data.Type) + Pose);
                                xWeapon.Activate();
                                xWeapon.Active = true;
                                xWeapon.Attack();
                            }
                        }
                        break;

                    case PlayerState.ZGuard:
                        m_Animator.Play("Guard");
                        {
                            rigidBody.LinearVelocity = Vector2.Zero;
                            zWeapon.Translation = new Vector3(12, -4, 0);
                            zWeapon.Rotation = new Vector3(0, 0, 0);
                            zWeapon.Activate();
                        }
                        break;
                    case PlayerState.XGuard:
                        m_Animator.Play("Guard");
                        {
                            rigidBody.LinearVelocity = Vector2.Zero;
                            xWeapon.Translation = new Vector3(12, -4, 0);
                            xWeapon.Rotation = new Vector3(0, 0, 0);
                            xWeapon.Activate();
                        }
                        break;

                    case PlayerState.ZGuardBreak:
                        m_Animator.Play("GuardBreak");
                        {
                            zWeapon.Translation = new Vector3(11.5f, -4, 0);
                            zWeapon.Rotation = new Vector3(0, 0, (float)Math.PI / 18);
                            m_AudioSource.Play("assets\\soundClip\\damage_05 #152416.wav", false);
                            zWeapon.Activate();
                        }
                        break;
                    case PlayerState.XGuardBreak:
                        m_Animator.Play("GuardBreak");
                        {
                            xWeapon.Translation = new Vector3(11.5f, -4, 0);
                            xWeapon.Rotation = new Vector3(0, 0, (float)Math.PI / 18);
                            m_AudioSource.Play("assets\\soundClip\\damage_05 #152416.wav", false);
                            xWeapon.Activate();
                        }
                        break;


                    case PlayerState.Damaged:
                        m_Animator.Play("Attacked");
                        m_AudioSource.Play("assets\\soundClip\\damage_a_05.wav", false);
                        blood.State = (uint)ParticleSystem.ParticleState.NORMAL;
                        invisible = true;
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
            rigidBody = GetComponent<Rigidbody2DComponent>();
            m_Animator = GetComponent<SpriteAnimatorComponent>();
            hitBox = GetComponent<BoxCollider2DComponent>();
            blood = GetComponent<ParticleSystem>();

            m_State = PlayerState.Idle;

            Vector3 pos = new Vector3(-460, 30, -5);
            DontDestroy(ref pos);

            var ui = FindEntityByName("Always Visilble UI");
            ui.DontDestroy();

            m_Inven = FindEntityByName("InventoryPanel");
            m_Inven.DontDestroy();

            m_AudioSource =GetComponent<AudioSource>();
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
                xWeapon = FindEntityByName("xWeapon").As<PlayerWeapon>();
            if (zWeapon == null)
                zWeapon = FindEntityByName("zWeapon").As<PlayerWeapon>();

            if (rigidBody.LinearVelocity.X > 1)
                if (State >= PlayerState.XGuard)
                    Flip = true;
                else
                    Flip = false;
            else if (rigidBody.LinearVelocity.X < -1)
                if (State >= PlayerState.XGuard)
                    Flip = false;
                else
                    Flip = true;
            stateTime += ts;
            UpdateState();
            if (m_State == PlayerState.Idle || m_State == PlayerState.Run || m_State == PlayerState.Aerial)
            {
                Vector2 velocity = rigidBody.LinearVelocity;
                velocity.X = 0;
                if (Input.IsKeyPressed(KeyCode.Right))
                    velocity.X += 1 * Speed;
                if (Input.IsKeyPressed(KeyCode.Left))
                    velocity.X -= 1 * Speed;

                rigidBody.LinearVelocity = velocity;

                if (Input.IsKeyDown(KeyCode.Space) && (m_State == PlayerState.Idle || m_State == PlayerState.Run))
                    rigidBody.ApplyLinearImpulse(new Vector2(0, 1200 * hitBox.Size.Y * hitBox.Size.X), true);
            }
            if (m_State == PlayerState.Roll)
            {
                float delta = stateTime / evadeTime;
                float targetSpeed = (1.0f - delta) * (evadeSpeed) + delta * Speed / 2.0f;
                if (Flip)
                    rigidBody.LinearVelocity = new Vector2(-targetSpeed, rigidBody.LinearVelocity.Y);
                else
                    rigidBody.LinearVelocity = new Vector2(targetSpeed, rigidBody.LinearVelocity.Y);
            }
            if (m_State == PlayerState.Damaged) {
                if (stateTime > 0.5f)
                {
                    blood.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                    invisible = false;
                    State = PlayerState.Idle;
                }
            }
        }

        private void UpdateState()
        {
            if (rigidBody.LinearVelocity.Y < 0)
            {
                if (isFalling == false && State == PlayerState.Aerial)
                    m_Animator.Play("Falling");
                isFalling = true;
            }
            else
                isFalling = false;

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
                        if (Input.IsKeyPressed(KeyCode.Down) == false && Math.Abs(rigidBody.LinearVelocity.X) < 1.0f)
                            State = PlayerState.Idle;
                        else if (Math.Abs(rigidBody.LinearVelocity.X) >= 1.0f && State != PlayerState.Run)
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
                        if (zWeapon.Data.Type != PlayerWeaponData.weaponType.Shield)
                            State = PlayerState.ZAttack;
                    if (Input.IsKeyPressed(KeyCode.Z) && zWeapon.Data != null)
                        if (zWeapon.Data.Type == PlayerWeaponData.weaponType.Shield)
                            State = PlayerState.ZGuard;

                    if (Input.IsKeyDown(KeyCode.X) && xWeapon.Data != null)
                        if (xWeapon.Data.Type != PlayerWeaponData.weaponType.Shield)
                            State = PlayerState.XAttack;
                    if (Input.IsKeyPressed(KeyCode.X) && xWeapon.Data != null)
                        if (xWeapon.Data.Type == PlayerWeaponData.weaponType.Shield)
                            State = PlayerState.XGuard;

                    break;
                case PlayerState.Roll:
                    if (stateTime > evadeTime)
                        State = PlayerState.Idle;
                    break;

                case PlayerState.ZAttack:
                case PlayerState.XAttack:
                    if (State == PlayerState.ZAttack && stateTime > zWeapon.Data.Duration ||
                        State == PlayerState.XAttack && stateTime > xWeapon.Data.Duration)
                    {
                        if (State == PlayerState.ZAttack)
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

                

                    else if (Input.IsKeyNone(KeyCode.Down) && isGrounded)
                    {
                        PlayerWeapon weapon;
                        if (State == PlayerState.ZAttack)
                            weapon = zWeapon;
                        else
                            weapon = xWeapon;

                        if (hitBox.Size.Y < 24)
                        {
                            Translation = new Vector3(Translation.X, Translation.Y + 5.0f, Translation.Z);
                            hitBox.Size = new Vector2(8, 24);

                            string key = "Attack" + Enum.GetName(typeof(PlayerWeaponData.weaponType), weapon.Data.Type) + "Standing";
                            m_Animator.Play(key, stateTime);
                        }

                        var trans = weapon.Translation;
                        trans.Y -= 2.0f;
                        weapon.Translation = trans;
                    }

                    else if (Input.IsKeyPressed(KeyCode.Down) && isGrounded)
                    {
                        PlayerWeapon weapon;
                        if (State == PlayerState.ZAttack)
                            weapon = zWeapon;
                        else
                            weapon = xWeapon;

                        if (hitBox.Size.Y > 19)
                        {
                            Translation = new Vector3(Translation.X, Translation.Y - 5.0f, Translation.Z);
                            hitBox.Size = new Vector2(8, 19);

                            string key = "Attack" + Enum.GetName(typeof(PlayerWeaponData.weaponType), weapon.Data.Type) + "Crouching";
                            m_Animator.Play(key, stateTime);
                        }

                        var trans = weapon.Translation;
                        trans.Y -= 5.0f;
                        weapon.Translation = trans;
                    }
                    break;

                case PlayerState.ZGuard:
                    if (!Input.IsKeyPressed(KeyCode.Z)) {
                        State = PlayerState.Idle;
                        zWeapon.Pause();
                    }
                    break;
                case PlayerState.XGuard:
                    if (!Input.IsKeyPressed(KeyCode.X)){
                        State = PlayerState.Idle;
                        xWeapon.Pause();
                    }
                    break;

                case PlayerState.XGuardBreak:
                case PlayerState.ZGuardBreak:
                    if (stateTime > 0.3f)
                    {
                        xWeapon.Pause();
                        zWeapon.Pause();
                        State = PlayerState.Idle;
                    }
                    break;

                default: break;
            }
        }
        void OnCollisionStay(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 0)) > 0)
            {
                Entity ground = new Entity(collsion.entityID);
                if (ground.Translation.Y < Translation.Y)
                    isGrounded = true;
            }
        }

        public override void OnAttacked(Weapon opp) {
            if (invisible || State == PlayerState.Damaged)
                return;
            xWeapon.Pause();
            zWeapon.Pause();
            if (opp.WorldPosition.X > Translation.X)
                rigidBody.ApplyLinearImpulse(new Vector2(-500 * hitBox.Size.Y * hitBox.Size.X, 250 * hitBox.Size.Y * hitBox.Size.X), true);
            else
                rigidBody.ApplyLinearImpulse(new Vector2(500 * hitBox.Size.Y * hitBox.Size.X, 250 * hitBox.Size.Y * hitBox.Size.X), true);

            State = PlayerState.Damaged;
        }

        public override void OnGuarded(Weapon opp)
        {
            if (!Flip)
                rigidBody.ApplyLinearImpulse(new Vector2(-400 * hitBox.Size.Y * hitBox.Size.X, 200 * hitBox.Size.Y * hitBox.Size.X), true);
            else
                rigidBody.ApplyLinearImpulse(new Vector2(400 * hitBox.Size.Y * hitBox.Size.X, 200 * hitBox.Size.Y * hitBox.Size.X), true);

            if (State == PlayerState.XGuard)
            {
                State = PlayerState.XGuardBreak;
                xWeapon.OnGuarded(opp.GetParent().As<Character>());
            }
            else
            {
                State = PlayerState.ZGuardBreak;
                zWeapon.OnGuarded(opp.GetParent().As<Character>());
            }
        }

        public override bool GuardCheck(Weapon opp)
        {
            if (State != PlayerState.XGuard && State != PlayerState.ZGuard)
                return false;

            if (opp.HasComponent<BoxCollider2DComponent>())
            {
                if (Flip && (opp.WorldPosition.X + opp.m_BoxCollider.Offset.X) < Translation.X)
                    return true;

                if (!Flip && (opp.WorldPosition.X - opp.m_BoxCollider.Offset.X) > Translation.X)
                    return true;
            }

            else if (opp.HasComponent<CircleCollider2DComponent>())
            {
                CircleCollider2DComponent col = opp.GetComponent<CircleCollider2DComponent>();
                if (Flip && (opp.WorldPosition.X + col.Offset.X) < Translation.X)
                    return true;

                if (!Flip && (opp.WorldPosition.X - col.Offset.X) > Translation.X)
                    return true;
            }
            return false;
        }

        void OnTriggerStay(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 5)) > 0 && Input.IsKeyPressed(KeyCode.Up))
            {
                Entity obj = new Entity(collsion.entityID);
                Interactable interactable = obj.As<Interactable>();
                if (interactable != null)
                    interactable.Interact();
            }
        }

        void OnCollisionExit(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 0)) > 0)
            {                 
                Entity ground = new Entity(collsion.entityID);
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

        void Crouching()
        {
            Translation = new Vector3(Translation.X, Translation.Y - 5.0f, Translation.Z);
            hitBox.Size = new Vector2(8, 19);
        }
    }
}