using SY;
using System;
using static SY.StateComponent;

namespace Sandbox
{
    public class BossController : Character
    {   
        private Entity[][] chain;
        private Entity[] cp;

        private Entity player;
        private Entity halo;
        private Entity eye;

        private Entity WingL;
        private Entity WingR;

        private ParticleSystem smallPS;
        private ParticleSystem largePS;

        private Entity Body;
        private Entity Hair;

        private Entity BossHpBar;

        public Entity BossSwordPref;
        private Entity[] PinkSword;

        private Entity warlockLightR;
        private Entity warlockLightL;

        private Entity warlockR;
        private Entity warlockL;

        private Entity summonedEyeL;
        private Entity summonedEyeR;

        private bool Invisible = false;

        private Vector3[] chainOffsets;

        public enum BossState
        { 
            Sleep,
            Chain,
            Appearing,
            Charge,
            Sword,
            Laser,
            Bullet,
            Attack,
            Attack2Idle,
            Idle,
            Idle2Charge,
        }

        private BossState _state;

        public BossState bossState
        {
            set{
                switch (value)
                {
                    case BossState.Appearing:
                        {
                            var particle = halo.GetComponent<ParticleSystem>();
                            particle.State = (uint)ParticleSystem.ParticleState.NORMAL;
                            BossHpBar.Activate();
                        }
                        break;

                    case BossState.Chain:
                        halo.Activate();
                        break;

                    case BossState.Charge:
                        halo.Pause();
                        break;

                    case BossState.Attack:
                        summonedEyeL.Activate();
                        summonedEyeR.Activate();
                        break;

                    case BossState.Attack2Idle:
                        WingL.Rotation = new Vector3();
                        WingR.Rotation = new Vector3();
                        smallPS.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                        largePS.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                        for (int i = 0; i < 3; i++)
                        {
                            string name = "BrightWingL" + i;
                            Entity lwing = FindEntityByName(name);
                            lwing.Pause();

                            name = "BrightWingR" + i;
                            Entity rwing = FindEntityByName(name);
                            rwing.Pause();
                        }
                        break;

                    case BossState.Idle:
                        {
                            halo.Activate();

                            Vector3 transR = warlockR.Translation;
                            transR.X = 216;
                            warlockR.Translation = transR;

                            Vector3 transL = warlockL.Translation;
                            transL.X = -24;
                            warlockL.Translation = transL;

                            warlockR.Activate();
                            warlockL.Activate();
                        }
                        break;

                    case BossState.Sword:
                        {
                            PinkSword[0].Activate();
                            PinkSword[1].Activate();

                            PinkSword[0].Rotation = new Vector3();
                            PinkSword[1].Rotation = new Vector3();                          
                        }
                        break;

                    default:
                        break;
                }

                _state = value;
                stateTime = 0.0f;
            }
            get{ return _state;  }
        }
        const float chainMoveTime = 2.5f;
        const float chainInterval = 1.0f;

        float attackedBefore = -1.0f;
        uint bossPhase = 1;

        public float HP
        {
            
            get {
                var bar = BossHpBar.GetComponent<SliderComponent>();
                return bar.Value; }
            set { hp = value;
                var bar = BossHpBar.GetComponent<SliderComponent>();
                bar.Value = hp;
            }
        }

        public float MaxHP
        {
            get {
                var bar = BossHpBar.GetComponent<SliderComponent>();
                return bar.maxValue; }
            set
            {
                var bar = BossHpBar.GetComponent<SliderComponent>();
                maxMp = value;
                bar.maxValue = hp;
            }
        }

        void OnCreate()
        {
            chain = new Entity[4][];
            cp = new Entity[4];
            chainOffsets = new Vector3[4];

            BossHpBar = FindEntityByName("BossHp");
            var bar = BossHpBar.GetComponent<SliderComponent>();
            bar.maxValue = 100;
            bar.Value = 90;

            for (int i=0;i< 4;i++)
            {
                chain[i] = new Entity[23];
                for (int j = 0; j < 22; j++)
                {
                    chain[i][j] = FindEntityByName("Chain" + (i + 1) + "-" + j);
                }
                chain[i][22] = FindEntityByName("Chain" + (i + 1) + "-end");

                cp[i] = FindEntityByName("Chain Particle" + (i+1));
            }

            chainOffsets[0] = new Vector3(-64, 200, 0.0f);
            chainOffsets[1] = new Vector3(64, 200, 0.0f);
            chainOffsets[2] = new Vector3(-18, 216, 0.0f);
            chainOffsets[3] = new Vector3(18, 216, 0.0f);

            halo = FindEntityByName("PlatformLight");
            bossState = BossState.Sleep;

            player = FindEntityByName("Player");
            eye = FindEntityByName("DarkLadyEye");

            WingL = FindEntityByName("BossWingL");
            WingR = FindEntityByName("BossWingR");

            smallPS = FindEntityByName("DarkLadyCirclelet").GetComponent<ParticleSystem>();
            largePS = FindEntityByName("HaloParticle").GetComponent<ParticleSystem>();

            Body = FindEntityByName("DarkLadyBody");
            Hair = FindEntityByName("DarkLadyHair");

            var bodyanim = Body.GetComponent<SpriteAnimatorComponent>();
            bodyanim.Play("IDLE");

            var hairAnim  = Hair.GetComponent<SpriteAnimatorComponent>();
            hairAnim.Play("HAIR");

            blood = Body.GetComponent<ParticleSystem>();

            PinkSword = new Entity[2];
            PinkSword[0] = BossSwordPref.Instantiate();
            PinkSword[0].Pause();

            PinkSword[1] = BossSwordPref.Instantiate();
            PinkSword[1].Pause();

            summonedEyeL = FindEntityByName("SummonedEyeL");
            summonedEyeR = FindEntityByName("SummonedEyeR");

            warlockLightR = FindEntityByName("WarlockLightR");
            warlockLightL = FindEntityByName("WarlockLightL");

            warlockR = FindEntityByName("WarlockR");
            warlockL = FindEntityByName("WarlockL");
        }

        void OnUpdate(float ts)
        {
            switch (bossState)
            {
                case BossState.Chain:
                    if (stateTime > 5.5f)
                        bossState = BossState.Appearing;

                    for (int i = 0; i < 4; i++)
                    {
                        if (stateTime + ts < chainMoveTime + chainInterval * i && stateTime + ts > chainInterval * i)
                        {
                            if (stateTime <= chainInterval * i)
                                cp[i].Activate();

                            chain[i][22].Translation = Vector3.Lerp(chain[i][22].Translation, Translation + chainOffsets[i], ts / (chainMoveTime + chainInterval * i - stateTime));
                        }
                    }
                    break;

                case BossState.Appearing:
                    if (stateTime > 4.5)
                        bossState = BossState.Charge;
                    else
                    {
                        WingL.Rotation = new Vector3(0, 0, 20.0f * stateTime / 4.5f * (float)Math.PI / 180.0f);
                        WingR.Rotation = new Vector3(0, 0, -20.0f * stateTime / 4.5f * (float)Math.PI / 180.0f);

                        Translation = Vector3.Lerp(Translation, new Vector3(96, -280, 2.0f), ts / (4.5f - stateTime));
                    }
                    break;

                case BossState.Charge:
                    if (stateTime > 2)
                    {
                        Invisible = false;

                        if (bossPhase == 1)
                            bossState = BossState.Attack;
                        else
                            bossState = BossState.Sword;

                        for (int i = 0; i < 3; i++)
                        {
                            string name = "BrightWingL" + i;
                            Entity lwing =  FindEntityByName(name);
                            lwing.Activate();

                            name = "BrightWingR" + i;
                            Entity rwing = FindEntityByName(name);
                            rwing.Activate();
                        }
                        smallPS.State = (uint)ParticleSystem.ParticleState.NORMAL;
                        largePS.State = (uint)ParticleSystem.ParticleState.NORMAL;
                    }
                    else
                    {
                        WingL.Rotation = new Vector3(0, 0, (2 - stateTime) * (float)Math.PI / 18.0f);
                        WingR.Rotation = new Vector3(0, 0, (stateTime - 2) * (float)Math.PI / 18.0f);
                    }
                    break;

                case BossState.Attack:
                    {
                        WingL.Rotation = new Vector3(0, 0, (float)(Math.PI / 36 * Math.Sin(stateTime * Math.PI)));
                        WingR.Rotation = new Vector3(0, 0, -(float)(Math.PI / 36 * Math.Sin(stateTime * Math.PI)));
                        Vector3 trans = Translation;
                        trans.Y = -280 + 5 * (float)Math.Sin(stateTime * Math.PI);
                        Translation = trans;
                    }
                    break;

                case BossState.Sword:
                    if (stateTime > 2)
                    {
                        bossState = BossState.Attack;
                        PinkSword[0].Pause();
                        PinkSword[1].Pause();
                    }

                    break;

                case BossState.Attack2Idle:
                    if (stateTime < 1)
                    {
                        Vector3 trans = Translation;
                        trans.Y = -312;
                        Translation = Vector3.Lerp(Translation, trans, ts / (1- stateTime));
                    }
                    else
                        bossState = BossState.Idle;
                    break;

                case BossState.Idle:
                    if (warlockR.GetComponent<StateComponent>().State == EntityState.Pause && warlockL.GetComponent<StateComponent>().State == EntityState.Pause)
                    {
                        bossState = BossState.Idle2Charge;
                        Console.WriteLine("Change");
                    }
                    break;

                case BossState.Idle2Charge:
                    if (stateTime > 2)
                        bossState = BossState.Charge;
                    else
                    {
                        WingL.Rotation = new Vector3(0, 0, stateTime * (float)Math.PI / 18);
                        WingR.Rotation = new Vector3(0, 0, -stateTime * (float)Math.PI / 18);

                        Translation = Vector3.Lerp(Translation, new Vector3(96, -280, 2.0f), ts / (2 - stateTime));
                    }
                    break;

            }
            UpdateEye();

            if (bossState != BossState.Chain && bossState != BossState.Sleep)
            { 
                for (int i = 0; i < 4; i++)
                {
                    chain[i][22].Translation = Translation + chainOffsets[i];
                }
            }

            if (attackedBefore >=0.0f) {
                attackedBefore += ts;
                if (attackedBefore > 0.25f)
                {
                    blood.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                    attackedBefore = -1.0f;
                }
            }

            stateTime += ts;
        }

        void UpdateEye() {

            Vector3 diff = player.WorldPosition - eye.WorldPosition;
            diff.Normalize();

            Entity pupil = FindEntityByName("DarkLadyPupil");

            pupil.Translation = diff * 2.0f;
        }

        void OnTriggerEnter(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 2)) > 0 && !Invisible)
            {
                attackedBefore = 0.0f;
                blood.State = (uint)ParticleSystem.ParticleState.NORMAL;

                Entity weapon = new Entity(collsion.entityID);
                if (weapon.WorldPosition.X > Translation.X)
                {
                    blood.VelocityBegin = new Vector2(100, 0);
                    blood.VelocityEnd = new Vector2(100, -50);
                }
                else
                {
                    blood.VelocityBegin = new Vector2(-100, 0);
                    blood.VelocityEnd = new Vector2(-100, -50);
                }

                HP -= 10;
                if (hp < (5 - bossPhase) * 20)
                {
                    Invisible = true;
                    bossState = BossState.Attack2Idle;
                    bossPhase += 1;

                    PinkSword[0].Pause();
                    PinkSword[1].Pause();

                    halo.GetComponent<ParticleSystem>().State = (uint)ParticleSystem.ParticleState.NORMAL;
                    
                    summonedEyeR.Pause();
                    summonedEyeL.Pause();

                    
                }
            }
        }
    }
}
