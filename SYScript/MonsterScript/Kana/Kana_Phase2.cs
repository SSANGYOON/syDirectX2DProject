using SY;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.AccessControl;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Kana_Phase2 : Character
    {
        private SpriteAnimatorComponent m_Animator;
        private SpriteRendererComponent m_Renderer;
        private SpriteRendererComponent m_WingRenderer;
        private SliderComponent hpBar;
        private AudioSource m_AudioSource;

        private Entity player;
        private Entity MainCamera;

        private Entity KanaBigSword;

        public Entity swordPref;
        private Entity[] smallSwords;

        private Random rand;
        private Entity tail;

        private float SwordCoolTime = 2f;
        private float TailCoolTime = 0.5f;

        private float bleedingTint = 0f;

        private uint _swordPhase;
        private float swordStateTime;
        private float SmallSwordCoolTime = 0f;
        private double SwordAngle;
        enum KanaState
        { 
            Transform,
            Falling,
            Idle,
            TailWhip, 
            Sword,
            Summon,
            Launch,
            Spin,
            Damaged,
            Dead
        }

        KanaState _state;

        KanaState State { get { return _state; }
            set {
                switch (value)
                { 
                    case KanaState.Idle:
                        m_Animator.Play("Idle2_Kana");

                        if (_state == KanaState.TailWhip)
                            TailCoolTime = 0.5f;
                        if (_state == KanaState.Sword)
                            SwordCoolTime = 2f;

                        break;
                    case KanaState.TailWhip:
                        m_Animator.Play("TailAttack");
                        break;

                    case KanaState.Sword:
                        {   
                            var ks = KanaBigSword.As<Kana_BigSword>();
                            ks.body.Enable = false;
                            KanaBigSword.Activate();
                            ks.Active = true;
                            KanaBigSword.Rotation = Vector3.Zero;
                            m_Animator.Play("SwordAttack");
                            break;
                        }

                    case KanaState.Summon:
                        {
                            m_Animator.Play("Summon");
                            break;
                        }
                    case KanaState.Launch:
                        {
                            m_Animator.Play("Launch");
                            break;
                        }
                    case KanaState.Spin:
                        {
                            m_Animator.Play("Spin");
                            m_AudioSource.Play("assets\\soundClip\\buff up attack (2).wav");
                            SwordPhase = 4;
                            break;
                        }

                    case KanaState.Damaged:
                        {
                            m_Animator.Play("Damaged_Kana2");
                            break;
                        }

                    case KanaState.Dead:
                        {
                            m_Animator.Play("Damaged_Kana2");
                            m_AudioSource.Stop();
                            MainCamera.GetComponent<AudioSource>().Stop();
                            Entity lastKana = FindEntityByName("Kana_Last");
                            lastKana.Translation = Translation;
                            lastKana.Activate();
                            m_Renderer.Shader = "SpriteFoward";
                            m_WingRenderer.Shader = "SpriteFoward";
                            KanaBigSword.Pause();
                            m_Renderer.Color = new Vector4(1, 1, 1, 1);
                            hpBar.Entity.Pause();
                            break;
                        }
                }
                
                _state = value;  stateTime = 0f; }
            
        }

        uint SwordPhase
        {
            get { return _swordPhase; }
            set { _swordPhase = value;
                swordStateTime = 0f;
                switch (value)
                {
                    case 0:
                        SwordAngle = 0;
                        for (int i = 0; i < 5; i++)
                        {
                            smallSwords[i].Rotation = Vector3.Zero;
                            smallSwords[i].Pause();
                        }
                        break;

                    case 1:
                        for (int i = 0; i < 5; i++)
                        {
                            SpriteRendererComponent sr = smallSwords[i].GetComponent<SpriteRendererComponent>();
                            Vector4 color = sr.Color;
                            color.W = 1f;
                            sr.Color = color;
                            smallSwords[i].Translation = Vector3.Zero;
                            smallSwords[i].Activate();
                        }
                        break;


                    default:
                        break;
                }
            }
        }
        void OnCreate()
        {
            Entity kana_sprite= FindEntityByName("Kana_Sprite");
            m_Animator = kana_sprite.GetComponent<SpriteAnimatorComponent>();
            player = FindEntityByName("Player");
            m_Renderer = kana_sprite.GetComponent<SpriteRendererComponent>();
            rigidBody = GetComponent<Rigidbody2DComponent>();
            hitBox = GetComponent<BoxCollider2DComponent>();
            Entity bar = FindEntityByName("BossHp");
            hpBar = bar.GetComponent<SliderComponent>();
            m_WingRenderer = FindEntityByName("Kana_wing").GetComponent<SpriteRendererComponent>();
            invisible = true;
            tail = FindEntityByName("Kana_tail");
            rand = new Random();
            MainCamera = FindEntityByName("MainCamera");
            KanaBigSword = FindEntityByName("Kana_big_sword");
            smallSwords = new Entity[5];
            m_AudioSource = GetComponent<AudioSource>();

            for (int i = 0; i < smallSwords.Length; i++)
            {
                smallSwords[i] = swordPref.Instantiate(Vector3.Zero, ID);
                smallSwords[i].Rotation = Vector3.Zero;
                smallSwords[i].Pause();
            }

            SwordPhase = 0;
            swordStateTime = 0f;
        }
        void OnActivated()
        {
            State = KanaState.Transform;
        }
        void OnUpdate(float ts)
        {
            switch (State)
            {
                case KanaState.Transform:
                    if (stateTime > 1.5f)
                    {
                        rigidBody.Enable = true;
                        State = KanaState.Falling;                     
                    }
                    break;
                case KanaState.Falling:
                    if (stateTime < 1f)
                    {
                        Vector4 color = m_Renderer.Color;
                        color.W = 1 - stateTime;
                        m_Renderer.Color = color;
                        m_Renderer.Emission = color;

                        m_WingRenderer.Color = color;
                        m_WingRenderer.Emission = color;

                        hpBar.Entity.Activate();
                        maxHp = 100;
                        hp = 100;
                        hpBar.maxValue = maxHp;
                        hpBar.Value = hp;
                    }
                    else
                    {
                        invisible = false;
                        State = KanaState.Idle;
                        m_Renderer.Color = new Vector4(1, 0, 0, 0);
                    }

                    break;
                case KanaState.Idle:
                    Vector2 diff = (player.Translation - Translation).XY;
                    if (SwordPhase == 3 && swordStateTime > 3)
                        State = KanaState.Spin;

                    else if (SmallSwordCoolTime <= 0 && SwordPhase == 0)
                        State = KanaState.Summon;

                    else if (SwordCoolTime <= 0 && Math.Abs(diff.X) < 60)
                    {
                        State = KanaState.Sword;
                        if (Flip)
                            KanaBigSword.Translation = new Vector3(player.Translation.X - 45f, -180f, -2f);
                        else
                            KanaBigSword.Translation = new Vector3(player.Translation.X + 45f, -180f, -2f);
                    }
                    else if (diff.X > 20)
                    {
                        Vector3 trans = Translation;
                        trans.X += 100 * ts;
                        Translation = trans;
                    }
                    else if (diff.X < -20)
                    {
                        Vector3 trans = Translation;
                        trans.X -= 100 * ts;
                        Translation = trans;
                    }
                    else if (stateTime > 0.5f)
                        State = KanaState.TailWhip;
                  
                    SwordCoolTime -= ts;

                    if (player.Translation.X > Translation.X)
                    {
                        if (!Flip)
                        {
                            for (int i = 0; i < 5; i++)
                            {
                                smallSwords[i].Rotation = new Vector3(0,0, -smallSwords[i].Rotation.Z);
                            }
                        }
                        Flip = true;
                    }
                    else
                    {
                        if (Flip)
                        {
                            for (int i = 0; i < 5; i++)
                            {
                                smallSwords[i].Rotation = new Vector3(0, 0, -smallSwords[i].Rotation.Z);
                            }
                        }
                        Flip = false;
                    }
                    break;

                case KanaState.Summon:
                    if (stateTime > 1)
                    {
                        SwordPhase = 1;
                        State = KanaState.Launch;
                        m_AudioSource.Play("assets\\soundClip\\buff up attack (2).wav");
                        
                    }
                    break;

                case KanaState.Launch:
                    if (stateTime > 1)
                        State = KanaState.Idle;
                    break;

                case KanaState.Spin:
                    if (stateTime > 1)
                        State = KanaState.Idle;
                    break;

                case KanaState.TailWhip:
                    if (stateTime < 0.4f && stateTime + ts > 0.4f)
                    {
                        tail.Translation = new Vector3(-17, -32, 1);
                        tail.Activate();
                    }

                    if (stateTime < 0.7f && stateTime + ts > 0.7f)
                    {
                        tail.Translation = new Vector3(0, -32, 1);
                        tail.Pause();
                    }

                    if (stateTime > 1f)
                        State = KanaState.Idle;
                    break;

                case KanaState.Sword:
                    if (stateTime < 0.4)
                    {
                        Vector3 swordTrans = KanaBigSword.Translation;
                        swordTrans.Y = -180 + 500 * stateTime;
                        KanaBigSword.Translation = swordTrans;
                    }
                    else if (stateTime < 0.8)
                    {
                        Vector3 swordRot = KanaBigSword.Rotation;
                        if (Flip)
                            swordRot.Z = (float)((stateTime - 0.4f) * 3.125 * Math.PI);
                        else
                            swordRot.Z = (float)((-stateTime + 0.4f) * 3.125 * Math.PI);
                        KanaBigSword.Rotation = swordRot;

                        if (stateTime + ts > 0.8)
                            KanaBigSword.As<Kana_BigSword>().body.Enable = true;
                    }
                    else if (stateTime < 1.2)
                    {
                        Vector3 swordRot = KanaBigSword.Rotation;
                        if (Flip)
                            swordRot.Z = (float)(1.25 * Math.PI  - 2.5 * (stateTime - 0.8) * Math.PI);
                        else
                            swordRot.Z = (float)(-1.25 * Math.PI  + 2.5 * (stateTime - 0.8) * Math.PI);
                        KanaBigSword.Rotation = swordRot;

                        Vector3 swordTrans = KanaBigSword.Translation;
                        swordTrans.Y = 20 - 60 * (stateTime - 1);
                        KanaBigSword.Translation = swordTrans;
                    }
                    else if(stateTime > 1.5){
                        State = KanaState.Idle;
                    }
                    break;
                case KanaState.Damaged:
                    if (stateTime > 0.5)
                        State = KanaState.Idle;
                    break;

                case KanaState.Dead:
                    if (stateTime > 1)
                        Pause();
                    else {
                        Vector4 color = m_Renderer.Color;
                        color.W = 1 - stateTime;
                        m_Renderer.Color = color;
                        m_WingRenderer.Color = color;
                    }
                    break;
            }

            float flip = Flip ? -1f : 1f;

            switch (SwordPhase)
            {
                case 1:
                    if (swordStateTime < 1)
                    {
                        for (int i = 0; i < 5; i++)
                        {
                            double angle = Math.PI * 0.4 * i;
                            smallSwords[i].Translation = new Vector3(flip * 120 * swordStateTime * (float)Math.Cos(angle), 10 + 120 * swordStateTime * (float)Math.Sin(angle), -0.1f);
                        }
                    }
                    else
                        SwordPhase = 2;

                    break;
                case 2:
                    if (swordStateTime < 1)
                    {
                        SwordAngle += Math.PI * ts * swordStateTime;
                        if (SwordAngle > Math.PI)
                            SwordAngle -= 2 * Math.PI;

                        for (int i = 0; i < 5; i++)
                        {
                            double angle = SwordAngle + Math.PI * 0.4 * i;
                            smallSwords[i].Translation = new Vector3(flip * 120 * (float)Math.Cos(angle), 10 + 120 * (float)Math.Sin(angle), -0.1f);
                        }
                    }
                    else {
                        SwordPhase = 3;
                    }
                    break;

                case 3:
                    SwordAngle += Math.PI * ts;
                    if (SwordAngle > Math.PI)
                        SwordAngle -= 2 * Math.PI;

                    for (int i = 0; i < 5; i++)
                    {
                        double angle = SwordAngle + Math.PI * 0.4 * i;
                        smallSwords[i].Translation = new Vector3(flip * 120 * (float)Math.Cos(angle), 10 + 120 * (float)Math.Sin(angle), -0.1f);
                    }
                    break;

                case 4:
                    if (swordStateTime < 1)
                    {
                        SwordAngle += Math.PI * ts;
                        if (SwordAngle > Math.PI)
                            SwordAngle -= 2 * Math.PI;
                        for (int i = 0; i < 5; i++)
                        {
                            double angle = SwordAngle + 0.4 * i * Math.PI;
                            double rotAngle = angle + 0.5 * Math.PI;

                            smallSwords[i].Translation = new Vector3(flip * 120 * (float)Math.Cos(angle), 10 + 120 * (float)Math.Sin(angle), -0.1f);
                            smallSwords[i].Rotation = Vector3.Lerp(smallSwords[i].Rotation, new Vector3(0, 0, flip * (float)rotAngle), ts / (1 - (float)swordStateTime));
                        }
                    }
                    else
                    {
                        SwordPhase = 5;
                        m_AudioSource.Play("assets\\soundClip\\Weapon_Woosh_06.wav", true);
                    }
                    break;

                case 5:
                    if (swordStateTime < 2)
                    {
                        SwordAngle += Math.PI * ts * 2;
                        if (SwordAngle > Math.PI)
                            SwordAngle -= 2 * Math.PI;
                        for (int i = 0; i < 5; i++)
                        {
                            double angle = SwordAngle + 0.4 * i * Math.PI;
                            double rotAngle = angle + 0.5 * Math.PI;

                            smallSwords[i].Translation = new Vector3(flip * 120 * (float)Math.Cos(angle), 10 + 120 * (float)Math.Sin(angle), -0.1f);
                            smallSwords[i].Rotation = new Vector3(0, 0, flip * (float)rotAngle);
                        }
                    }
                    else
                    {
                        SwordPhase = 6;
                        m_AudioSource.Stop();
                    }
                    break;

                case 6:
                    if (swordStateTime < 2)
                    {
                        SwordAngle += Math.PI * ts * (2 - swordStateTime);
                        if (SwordAngle > Math.PI)
                            SwordAngle -= 2 * Math.PI;
                        for (int i = 0; i < 5; i++)
                        {
                            double angle = SwordAngle + 0.4 * i * Math.PI;
                            double rotAngle = angle + 0.5 * Math.PI;

                            smallSwords[i].Translation = new Vector3(flip * 120 * (float)Math.Cos(angle), 10 + 120 * (float)Math.Sin(angle), -0.1f);
                            smallSwords[i].Rotation = new Vector3(0, 0, flip * (float)rotAngle);

                            SpriteRendererComponent sr = smallSwords[i].GetComponent<SpriteRendererComponent>();
                            Vector4 color = sr.Color;
                            color.W = (2 - swordStateTime) / 2;
                            sr.Color = color;
                        }
                    }
                    else
                    {
                        SmallSwordCoolTime = 5;
                        SwordPhase = 0;
                    }

                    break;
            }

            if (bleedingTint > 0 && State != KanaState.Dead)
            {
                Vector4 color = m_Renderer.Color;
                color.W = (float)(-Math.Cos(bleedingTint * Math.PI * 2 * 8) / 2 + 0.5);
                m_Renderer.Color = color;
                bleedingTint -= ts;
            }

            hpBar.Value = hp;

            if (State < KanaState.Summon)
                SmallSwordCoolTime -= ts;

            swordStateTime += ts;
            stateTime += ts;
        }

        public override void OnAttacked(Weapon opp)
        {
            if (State == KanaState.Idle)
            {
                State = KanaState.Damaged;
                if (Flip)
                    rigidBody.ApplyLinearImpulse(new Vector2(-600 * hitBox.Size.Y * hitBox.Size.X, 300 * hitBox.Size.Y * hitBox.Size.X), true);
                else
                    rigidBody.ApplyLinearImpulse(new Vector2(600 * hitBox.Size.Y * hitBox.Size.X, 300 * hitBox.Size.Y * hitBox.Size.X), true);
            }
            else if (State > KanaState.Idle && State < KanaState.Dead)
            {
                if (Flip)
                    rigidBody.ApplyLinearImpulse(new Vector2(-300 * hitBox.Size.Y * hitBox.Size.X, 150 * hitBox.Size.Y * hitBox.Size.X), true);
                else
                    rigidBody.ApplyLinearImpulse(new Vector2(300 * hitBox.Size.Y * hitBox.Size.X, 150 * hitBox.Size.Y * hitBox.Size.X), true);
            }

            else if (State == KanaState.Dead)
            {
                invisible = true;
                if (Flip)
                    rigidBody.ApplyLinearImpulse(new Vector2(-600 * hitBox.Size.Y * hitBox.Size.X, 600 * hitBox.Size.Y * hitBox.Size.X), true);
                else
                    rigidBody.ApplyLinearImpulse(new Vector2(600 * hitBox.Size.Y * hitBox.Size.X, 600 * hitBox.Size.Y * hitBox.Size.X), true);

                Timer.SetTimeScale(0.5f);
            }
            bleedingTint = 0.5f;
        }

        public override void ReceiveDamage(float damage)
        {
            hp -= damage;
            if (hp <= 0)
            {
                hp = 0;
                State = KanaState.Dead;
            }
        }
    }
}
