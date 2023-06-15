using Microsoft.Win32;
using SY;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Security;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Kana_Phase1 : Character
    {
        SpriteAnimatorComponent m_Animator;
        SpriteRendererComponent m_Renderer;
        SpriteRendererComponent m_HumanBodyRenderer;
        Entity player;
        SliderComponent hpBar;


        private float groggy = 0f;
        private float bleedingTint = 0f;
        enum Kana_State
        {
            Human,
            FormChange,
            Idle,
            Trace,
            Slash,
            RushBegin,
            Rush,
            RushEnd,
            Guard,
            Blocking,
            Parrying,
            Parried,
            Damaged,
            Dead,
            PhaseChange
        }

        private Kana_State m_State;
        private ParticleSystem m_DarkParticle;
        private KanaKatana weapon;

        private Entity MainCamera;

        private bool superAmor = false;

        private uint remainNormalAttack;
        Kana_State State
        {
            set {
                switch (value) {
                    case Kana_State.Idle:
                        m_Animator.Play("Idle_DEMON");
                        if (m_State == Kana_State.RushEnd)
                        {
                            groggy = 1f;
                            remainNormalAttack = 3;
                        }
                        else if (m_State == Kana_State.Slash || m_State == Kana_State.Parried)
                            groggy = 0.5f;
                        break;
                    case Kana_State.Trace:
                        m_Animator.Play("Run_Kana");
                        break;

                    case Kana_State.Slash:
                        m_Animator.Play("Attack_Start_Kana");
                        remainNormalAttack--;
                        if (weapon == null)
                            weapon = FindEntityByName("Kana_HumanBody").As<KanaKatana>();
                        break;

                    case Kana_State.Parried:
                        m_Animator.Play("Blocked_Kana");
                        break;

                    case Kana_State.RushBegin:
                        m_Animator.Play("DashAttackStart");
                        superAmor = true;
                        break;
                    case Kana_State.Rush:
                        if(weapon == null)
                            weapon = FindEntityByName("Kana_HumanBody").As<KanaKatana>();
                        weapon.Active = true;
                        weapon.IgnoreGuard = true;
                        break;
                    case Kana_State.RushEnd:
                        weapon.Active = false;
                        superAmor = false;
                        rigidBody.LinearVelocity = new Vector2(0, 0);
                        weapon.IgnoreGuard = false;
                        break;

                    case Kana_State.Damaged:
                        m_Animator.Play("Damaged_Kana");
                        break;

                    case Kana_State.Dead:
                        m_Animator.Play("Dead_Kana");
                        invisible = true;
                        var camScript = MainCamera.As<Camera>();
                        MainCamera.As<Camera>().Cinematic = true;
                        camScript.Following = this;
                        m_DarkParticle.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                        Timer.SetTimeScale(0.5f);
                        hpBar.Entity.Pause();
                        break;

                    case Kana_State.PhaseChange:
                        rigidBody.Enable = false;
                        m_Animator.Play("Kana_Transform");
                        break;
                }
                  
                m_State = value; stateTime = 0f; 
            
            }
            get { return m_State; }
        }
        void OnCreate()
        {
            m_Animator = GetComponent<SpriteAnimatorComponent>();
            m_Animator.Play("Idle_DEMON");
            State = Kana_State.Human;
            var humanBody = FindEntityByName("Kana_HumanBody");
            humanBody.GetComponent<SpriteAnimatorComponent>().Play("Idle_HUMAN");
            player = FindEntityByName("Player");
            m_HumanBodyRenderer = humanBody.GetComponent<SpriteRendererComponent>();
            m_Renderer = GetComponent<SpriteRendererComponent>();
            m_DarkParticle = humanBody.GetComponent<ParticleSystem>();
            rigidBody = GetComponent<Rigidbody2DComponent>();
            hitBox = GetComponent<BoxCollider2DComponent>();
            remainNormalAttack = 3;
            Entity bar = FindEntityByName("BossHp");
            hpBar = bar.GetComponent<SliderComponent>();
            maxHp = 100;
            hp = 100;
            hpBar.maxValue = maxHp;
            hpBar.Value = hp;
            invisible = true;

            MainCamera = FindEntityByName("MainCamera");
        }

        public void FormChange()
        {
            State = Kana_State.FormChange;
            var camScript = MainCamera.As<Camera>();
            MainCamera.As<Camera>().Cinematic = true;
            camScript.Following = this;
        }
        void OnUpdate(float ts)
        {
            switch (State)
            {
                case Kana_State.FormChange:

                    if (stateTime < 6)
                    {
                        m_Renderer.SetFloat(0, stateTime * 10);
                        m_HumanBodyRenderer.SetFloat(0, stateTime * 10);
                    }
                    else
                    {
                        m_Renderer.Shader = "TintShader";
                        var humanBody = FindEntityByName("Kana_HumanBody");
                        humanBody.Translation = new Vector3(0, 0, 1);
                        State = Kana_State.Idle;
                        m_DarkParticle.State = (uint)ParticleSystem.ParticleState.NORMAL;
                        m_HumanBodyRenderer.Color = new Vector4();
                        FindEntityByName("BossHp").Activate();
                        invisible = false;

                        var Cam = MainCamera.GetComponent<CameraComponent>();
                        MainCamera.As<Camera>().Cinematic = false;
                    }
                    break;
                case Kana_State.Idle:
                    if (groggy < 0f)
                    {
                        if (Math.Abs(player.Translation.X - Translation.X) > 150 || remainNormalAttack <=0)
                            State = Kana_State.RushBegin;
                        else
                            State = Kana_State.Trace;
                    }
                    else
                        groggy -= ts;
                    break;

                case Kana_State.Trace:
                    if (Math.Abs(player.Translation.X - Translation.X) > 150)
                        State = Kana_State.RushBegin;
                    else if (player.Translation.X - Translation.X > 42)
                        rigidBody.LinearVelocity = new Vector2(100, 0);
                    else if (player.Translation.X - Translation.X < -42)
                        rigidBody.LinearVelocity = new Vector2(-100, 0);
                    else
                    {
                        rigidBody.LinearVelocity = Vector2.Zero;
                        State = Kana_State.Slash;
                    }
                    break;

                case Kana_State.Slash:
                    if (stateTime >= 1.2f)
                        State = Kana_State.Idle;
                    else if (stateTime + ts > 0.6 && stateTime < 0.6)
                    {
                        superAmor = true;
                        weapon.Active = true;
                    }
                    else if (stateTime + ts > 0.7 && stateTime < 0.7)
                    {
                        superAmor = false;
                        weapon.Active = false;
                    }
                    break;

                case Kana_State.RushBegin:
                    if (stateTime < 0.5f)
                    {
                        if (Flip)
                            rigidBody.LinearVelocity = new Vector2(-100, 0);
                        else
                            rigidBody.LinearVelocity = new Vector2(100, 0);
                    }
                    else
                        State = Kana_State.Rush;
                    break;

                case Kana_State.Rush:
                    if (stateTime < 0.75f)
                    {
                        if (Flip)
                            rigidBody.LinearVelocity = new Vector2(400, 0);
                        else
                            rigidBody.LinearVelocity = new Vector2(-400, 0);
                    }
                    else
                        State = Kana_State.RushEnd;
                    break;

                case Kana_State.RushEnd:
                    if (stateTime > 0.2f)
                        State = Kana_State.Idle;

                    break;

                case Kana_State.Parried:
                    if (stateTime > 0.5f)
                        State = Kana_State.Idle;
                    break;

                case Kana_State.Damaged:
                    if (stateTime > 0.5f)
                        State = Kana_State.Idle;
                    break;

                case Kana_State.Dead:
                    {
                        if (stateTime + ts > 1f)
                            Timer.SetTimeScale(1f);

                        if (stateTime > 3f)
                            State = Kana_State.PhaseChange;
                    }
                    break;

                case Kana_State.PhaseChange:
                    {
                        Vector3 trans = Translation;
                        trans.Y = 50;

                        if (stateTime > 0.4f && stateTime < 3f)
                        {
                            Translation = Vector3.Lerp(Translation, trans, ts / (3f - stateTime));
                            if (stateTime + ts > 3)
                                GetComponent<ParticleSystem>().State = (uint)ParticleSystem.ParticleState.NORMAL;
                        }

                        else if (stateTime > 3f && stateTime < 5f)
                        {
                            m_Renderer.Color = new Vector4(1, 1, 1, (stateTime - 3) / 2);
                            m_Renderer.Emission = new Vector4(1, 1, 1, (stateTime - 3) / 2);
                            if (stateTime + ts > 5)
                            {
                                
                                GetComponent<ParticleSystem>().VelocityBegin = new Vector2(200, 0);
                                GetComponent<ParticleSystem>().VelocityEnd = new Vector2(200, 0);
                                GetComponent<ParticleSystem>().State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                                m_Animator.Stop();                            
                            }
                        }

                        else if (stateTime  + ts> 6.5f &&stateTime  < 6.5f)
                        {
                            GetComponent<ParticleSystem>().State = (uint)ParticleSystem.ParticleState.NORMAL;

                            Entity phase2 = FindEntityByName("Kana_Phase2");
                            m_Renderer.tile = new Vector2(1, 1);
                            phase2.Activate();
                            phase2.GetComponent<Rigidbody2DComponent>().Enable = false;
                            phase2.Translation = Translation;
                        }

                        else if (stateTime  + ts> 7f && stateTime < 7f)
                        {
                            GetComponent<ParticleSystem>().State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;                          
                        }
                        if (stateTime > 8)
                        {
                            Pause();
                            var camScript = MainCamera.As<Camera>();
                            camScript.Cinematic = false;
                        }

                    }
                    break;

            }
            if (State >= Kana_State.Idle)
            {
                Vector2 tile = m_Renderer.tile;
                Vector2 offset = m_Renderer.offset;
                m_DarkParticle.AttachOffset = offset + tile / 2;
                m_DarkParticle.PositionVariation = tile;
            }
            if (State == Kana_State.Idle || State == Kana_State.Trace)
            {
                if (player.Translation.X > Translation.X)
                    Flip = true;
                else
                    Flip = false;
            }

            if (bleedingTint > 0)
            {
                Vector4 color = m_Renderer.Color;
                color.W = (float)(-Math.Cos(bleedingTint * Math.PI * 2 * 8) / 2 + 0.5);
                m_Renderer.Color = color;
                bleedingTint -= ts;
            }
            hpBar.Value = hp;
            stateTime += ts;
        }
        public override void OnBlocked()
        {
            State = Kana_State.Parried;
            superAmor = false;
            if (Flip)
                rigidBody.ApplyLinearImpulse(new Vector2(-400 * hitBox.Size.Y * hitBox.Size.X, 200 * hitBox.Size.Y * hitBox.Size.X), true);
            else
                rigidBody.ApplyLinearImpulse(new Vector2(400 * hitBox.Size.Y * hitBox.Size.X, 200 * hitBox.Size.Y * hitBox.Size.X), true);
        }

        public override void OnAttacked(Weapon opp)
        {
            if (State != Kana_State.Dead && !superAmor)
            {
                State = Kana_State.Damaged;
                if (Flip)
                    rigidBody.ApplyLinearImpulse(new Vector2(-600 * hitBox.Size.Y * hitBox.Size.X, 300 * hitBox.Size.Y * hitBox.Size.X), true);
                else
                    rigidBody.ApplyLinearImpulse(new Vector2(600 * hitBox.Size.Y * hitBox.Size.X, 300 * hitBox.Size.Y * hitBox.Size.X), true);
            }
            else
            {
                if (Flip)
                    rigidBody.ApplyLinearImpulse(new Vector2(-600 * hitBox.Size.Y * hitBox.Size.X, 1200 * hitBox.Size.Y * hitBox.Size.X), true);
                else
                    rigidBody.ApplyLinearImpulse(new Vector2(600 * hitBox.Size.Y * hitBox.Size.X, 1200 * hitBox.Size.Y * hitBox.Size.X), true);
            }

            bleedingTint = 0.5f; 
        }

        public override void ReceiveDamage(float damage)
        {
            base.ReceiveDamage(damage);
            if (hp <= 0)
            {
                hp = 0;
                State = Kana_State.Dead;
            }
        }

    }
}
