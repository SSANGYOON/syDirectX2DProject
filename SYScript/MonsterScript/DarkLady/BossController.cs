using SY;
using System;
using System.Collections;
using static SY.StateComponent;

namespace Sandbox
{
    public class BossController : Character
    {   
        private SpriteRendererComponent[,] chainSp;
        private Entity[] chainBase;
        private Entity[] chainEnd;
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

        private Vector3[] chainOffsets;

        private Entity BulletGeneratorL;
        private Entity BulletGeneratorR;

        private Entity pupil;

        private Entity HolyBody;
        private Entity HolyWingL;
        private Entity HolyWingR;

        private Entity[] Lasers;

        private SpriteRendererComponent[] BrightWingL;
        private SpriteRendererComponent[] BrightWingR;

        private float[] targetLength;

        SpriteRendererComponent BodySp;

        SpriteRendererComponent WingLSp;

        SpriteRendererComponent WingRSp;

        SpriteRendererComponent eyeSp;

        SpriteRendererComponent pupilSp;

        SpriteRendererComponent HolyBodySp;

        SpriteRendererComponent HolyWingLSp;

        SpriteRendererComponent HolyWingRSp;

        AudioSource _audioSource;

        public enum BossState
        { 
            Sleep,
            Chain,
            Appearing,
            Charge,
            Sword,
            Bullet,
            Laser,
            Attack,
            Attack2Idle,
            Idle,
            Idle2Charge,
            OutBurst,
            Falling,
            ChainDissolve,
            Miracle,
            Cleanse,
            HolyCharge,
            HolyEnd,
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

                            FindEntityByName("MainCamera").GetComponent<AudioSource>().Play("assets/soundClip/trailer-guitar.wav", true);
                        }
                        break;

                    case BossState.Chain:
                        halo.Activate();
                        break;

                    case BossState.Charge:
                        for (int i = 0; i < 4; i++)
                        {
                            var dj = chainBase[i].GetComponent<DistanceJointComponent>();
                            targetLength[i] = (dj.CurrentLength) * 1.2f;

                            dj.MinLength = dj.CurrentLength;
                        }
                        halo.Pause();
                        break;

                    case BossState.Attack:
                        summonedEyeL.Activate();
                        summonedEyeR.Activate();
                        Invisible = false;
                        break;

                    case BossState.Attack2Idle:
                        WingL.Rotation = new Vector3();
                        WingR.Rotation = new Vector3();
                        smallPS.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                        largePS.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                        for (int i = 0; i < 3; i++)
                        {
                            BrightWingL[i].Entity.Pause();
                            BrightWingR[i].Entity.Pause();
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

                            warlockLightL.Activate();
                            warlockLightR.Activate();
                        }
                        break;

                    case BossState.Sword:
                        {
                            PinkSword[0].Activate();
                            PinkSword[1].Activate();

                            PinkSword[0].Rotation = new Vector3();
                            PinkSword[1].Rotation = new Vector3();
                            Invisible = false;
                        }
                        break;

                    case BossState.Bullet:
                        {
                            BulletGeneratorL.Activate();
                            BulletGeneratorL.Translation = eye.WorldPosition + new Vector3(-80, 0, -2);
                            BulletGeneratorL.Rotation = new Vector3();

                            BulletGeneratorR.Activate();
                            BulletGeneratorR.Translation = eye.WorldPosition + new Vector3(80, 0, -2);
                            BulletGeneratorR.Rotation = new Vector3(0,0,(float)Math.PI/3 * 2);
                            var br = BulletGeneratorR.As<BulletGenerator>();
                            br.reverse = true;
                            Invisible = false;
                        }
                        break;

                    case BossState.Laser:
                        for (int i = 0; i < Lasers.Length; i++)
                        {
                            Lasers[i].Activate();
                        }

                        _audioSource.Play("assets\\soundClip\\CatacombBeam.wav", true);
                        Invisible = false;
                        break;

                    case BossState.OutBurst:
                        Timer.SetTimeScale(1f);
                        halo.Activate();
                        break;

                    case BossState.Falling:
                        halo.Pause();
                        _audioSource.Play("assets\\soundClip\\Holy (1).wav", false);
                        for (int i = 0; i < 4; i++)
                        {
                            for (int j = 0; j < 23; j++)
                            {
                                if (i > 1 && (j == 20 || j == 21))
                                    continue;

                                if (chainSp[i, j] != null)
                                {
                                    chainSp[i, j].SetFloat(1, 0.2f);
                                    chainSp[i, j].Emission = new Vector4(1, 0.33333f, 0.1060f, 1);
                                }
                            }
                        }
                        break;

                    case BossState.ChainDissolve:
                        for (int i = 0; i < 4; i++)
                        {
                            cp[i].Pause();
                            var dj = chainBase[i].GetComponent<DistanceJointComponent>();
                            dj.MinLength = dj.CurrentLength;
                            chainEnd[i].GetComponent<Rigidbody2DComponent>().Type = Rigidbody2DComponent.BodyType.Dynamic;
                        }
                        break;

                    case BossState.Miracle:
                        halo.Activate();
                        halo.GetComponent<SpriteRendererComponent>().Color = new Vector4(1, 241F / 255F, 106F / 255F, 1);
                        var haloPs = halo.GetComponent<ParticleSystem>();
                        haloPs.EmissionBegin = new Vector4(1, 239F / 255F, 30F / 255F, 1);
                        haloPs.EmissionEnd = new Vector4(1, 239F / 255F, 30F / 255F, 0);

                        haloPs.ColorBegin = new Vector4(1, 239F / 255F, 30F / 255F, 1);
                        haloPs.ColorEnd = new Vector4(1, 239F / 255F, 30F / 255F, 0);

                        FindEntityByName("MainCamera").GetComponent<AudioSource>().Play("assets\\soundClip\\heaven.wav", true);

                        break;

                    case BossState.Cleanse:
                        Vector2 targetPos = FindEntityByName("DarkLadyCirclelet").WorldPosition.XY;

                        var circlet = FindEntityByName("DarkLadyCirclelet");
                        var tr = FindEntityByName("DarkLadyCirclelet").Translation;
                        tr.Z = -10;
                        circlet.Translation = tr;

                        BodySp.SetVec2(0, ref targetPos);
                        WingLSp.SetVec2(0, ref targetPos);
                        WingRSp.SetVec2(0, ref targetPos);
                        eyeSp.SetVec2(0, ref targetPos);
                        pupilSp.SetVec2(0, ref targetPos);
                        HolyBodySp.SetVec2(0, ref targetPos);
                        HolyWingLSp.SetVec2(0, ref targetPos);
                        HolyWingRSp.SetVec2(0, ref targetPos);

                        smallPS.State = (uint)ParticleSystem.ParticleState.NORMAL;
                        smallPS.Position = new Vector2(-1, -90);
                        smallPS.PositionVariation = new Vector2(1, 120);
                        smallPS.VelocityBegin = new Vector2(60, 0);
                        smallPS.VelocityEnd = new Vector2(60, 0);

                        smallPS.EmissionBegin = new Vector4(1, 239F / 255F, 30F / 255F, 1);
                        smallPS.EmissionEnd = new Vector4(1, 239F / 255F, 30F / 255F, 0);

                        smallPS.ColorBegin = new Vector4(1, 239F / 255F, 30F / 255F, 1);
                        smallPS.ColorEnd = new Vector4(1, 239F / 255F, 30F / 255F, 0);
                        break;
                    case BossState.HolyCharge:
                        smallPS.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                        break;
                    case BossState.HolyEnd:
                        _audioSource.Play("assets\\soundClip\\Holy (8).wav", false);
                        for (int i = 0; i < 4; i++)
                        {
                            BrightWingL[i].Entity.Activate();
                            BrightWingR[i].Entity.Activate();

                            BrightWingL[i].Color = new Vector4(1, 1, 0.2f, 1);
                            BrightWingL[i].Emission = new Vector4(1, 1, 0.2f, 1);

                            BrightWingR[i].Color = new Vector4(1, 1, 0.2f, 1);
                            BrightWingR[i].Emission = new Vector4(1, 1, 0.2f, 1);
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
            chainSp = new SpriteRendererComponent[4, 23];
            chainEnd = new Entity[4];
            cp = new Entity[4];
            chainOffsets = new Vector3[4];

            BossHpBar = FindEntityByName("BossHp");
            var bar = BossHpBar.GetComponent<SliderComponent>();
            bar.maxValue = 100;
            bar.Value = 90;

            for (int i=0;i< 4;i++)
            {
                for (int j = 0; j < 22; j++)
                {
                    Entity cha = FindEntityByName("Chain" + (i + 1) + "-" + j);
                    if (cha != null)
                    {
                        chainSp[i, j] = cha.GetComponent<SpriteRendererComponent>();
                        chainSp[i, j].SetFloat(0, 9);
                        chainSp[i, j].SetFloat(1, 1);
                    }

                }
                chainEnd[i] = FindEntityByName("Chain" + (i + 1) + "-end");
                chainSp[i, 22] = chainEnd[i].GetComponent<SpriteRendererComponent>();
                chainSp[i, 22].SetFloat(0, 9);
                chainSp[i, 22].SetFloat(1, 1);

                cp[i] = FindEntityByName("Chain Particle" + (i+1));
            }

            chainOffsets[0] = new Vector3(-64, 200, 0.0f);
            chainOffsets[1] = new Vector3(64, 200, 0.0f);
            chainOffsets[2] = new Vector3(-18, 216, 0.0f);
            chainOffsets[3] = new Vector3(18, 216, 0.0f);

            chainBase = new Entity[4];

            for (int i = 0; i < 4; i++)
            {
                chainBase[i] = FindEntityByName("Chain" + (i + 1) + "-Base");
            }

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

            HolyBody = FindEntityByName("HolyBody");
            HolyWingL = FindEntityByName("HolyWingL");
            HolyWingR = FindEntityByName("HolyWingR");

            var bodyanim = Body.GetComponent<SpriteAnimatorComponent>();
            bodyanim.Play("IDLE");

            var hairAnim  = Hair.GetComponent<SpriteAnimatorComponent>();
            hairAnim.Play("HAIR");

            blood = Body.GetComponent<ParticleSystem>();

            PinkSword = new Entity[2];
            PinkSword[0] = BossSwordPref.Instantiate();
            PinkSword[0].GetComponent<Rigidbody2DComponent>().Enable = false;
            PinkSword[0].Pause();

            PinkSword[1] = BossSwordPref.Instantiate();
            PinkSword[1].GetComponent<Rigidbody2DComponent>().Enable = false;
            PinkSword[1].Pause();

            summonedEyeL = FindEntityByName("SummonedEyeL");
            summonedEyeR = FindEntityByName("SummonedEyeR");

            warlockLightR = FindEntityByName("WarlockLightR");
            warlockLightL = FindEntityByName("WarlockLightL");

            warlockR = FindEntityByName("WarlockR");
            warlockL = FindEntityByName("WarlockL");
            pupil = FindEntityByName("DarkLadyPupil");
            BulletGeneratorL = FindEntityByName("BulletGeneratorL");
            BulletGeneratorR= FindEntityByName("BulletGeneratorR");

            Lasers = new Entity[4];

            for (int i = 0; i < Lasers.Length; i++)
            {
                Lasers[i] = FindEntityByName("Laser" + i);
                Lasers[i].Pause();
            }

            targetLength = new float[4];

            BodySp = Body.GetComponent<SpriteRendererComponent>();
            var BossDarkWingL = FindEntityByName("BossDarkWingL");
            WingLSp = BossDarkWingL.GetComponent<SpriteRendererComponent>();
            var BossDarkWingR = FindEntityByName("BossDarkWingR");
            WingRSp = BossDarkWingR.GetComponent<SpriteRendererComponent>();
            eyeSp = eye.GetComponent<SpriteRendererComponent>();
            pupilSp = pupil.GetComponent<SpriteRendererComponent>();
            HolyBodySp = HolyBody.GetComponent<SpriteRendererComponent>();
            HolyWingLSp = HolyWingL.GetComponent<SpriteRendererComponent>();
            HolyWingRSp = HolyWingR.GetComponent<SpriteRendererComponent>();

            BrightWingL = new SpriteRendererComponent[4];
            BrightWingR = new SpriteRendererComponent[4];
            for (int i = 0; i < 4; i++)
            {
                string name = "BrightWingL" + i;
                BrightWingL[i] = FindEntityByName(name).GetComponent<SpriteRendererComponent>();

                name = "BrightWingR" + i;
                BrightWingR[i] = FindEntityByName(name).GetComponent<SpriteRendererComponent>();
            }

            _audioSource = GetComponent<AudioSource>();
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
                            {
                                cp[i].Activate();
                                _audioSource.Play("assets\\soundClip\\magical whoosh (1).wav");
                            }
                                

                            chainEnd[i].Translation = Vector3.Lerp(chainEnd[i].Translation, Translation + chainOffsets[i], ts / (chainMoveTime + chainInterval * i - stateTime));
                        }
                    }
                    break;

                case BossState.Appearing:
                    if (stateTime > 3)
                        bossState = BossState.Charge;
                    else
                    {
                        WingL.Rotation = new Vector3(0, 0, 20.0f * stateTime / 3f * (float)Math.PI / 180.0f);
                        WingR.Rotation = new Vector3(0, 0, -20.0f * stateTime / 3f * (float)Math.PI / 180.0f);

                        Translation = Vector3.Lerp(Translation, new Vector3(96, -280, 2.0f), ts / (3f - stateTime));
                    }
                    break;

                case BossState.Charge:
                    if (stateTime > 1)
                    {

                        if (bossPhase == 1)
                            bossState = BossState.Attack;
                        else if (bossPhase == 2)
                            bossState = BossState.Sword;
                        else if (bossPhase == 3)
                            bossState = BossState.Bullet;
                        else
                            bossState = BossState.Laser;


                        for (int i = 0; i < 3; i++)
                        {
                            BrightWingL[i].Entity.Activate();
                            BrightWingR[i].Entity.Activate();
                        }

                        smallPS.State = (uint)ParticleSystem.ParticleState.NORMAL;
                        largePS.State = (uint)ParticleSystem.ParticleState.NORMAL;
                    }
                    else
                    {
                        WingL.Rotation = new Vector3(0, 0, (1 - stateTime) * (float)Math.PI / 9.0f);
                        WingR.Rotation = new Vector3(0, 0, (stateTime - 1) * (float)Math.PI / 9.0f);

                        for (int i = 0; i < 4; i++)
                        {
                            var dj = chainBase[i].GetComponent<DistanceJointComponent>();
                            float length = chainBase[i].GetComponent<DistanceJointComponent>().MinLength;

                            dj.MinLength = (targetLength[i] - length) * ts / (1 - stateTime) + length;


                            for (int j = 0; j < 23; j++)
                            {
                                if (i > 1 && (j == 20 || j == 21))
                                    continue;

                                if (chainSp[i, j] != null)
                                {
                                    chainSp[i, j].Emission = new Vector4(1, 0.33333f, 0.1060f, 1 - stateTime);
                                    chainSp[i, j].SetFloat(0, 9);
                                    chainSp[i, j].SetFloat(1, 1);
                                }   
                            }

                        }
                    }
                    break;

                case BossState.Sword:
                    Vector3 eyePos = eye.WorldPosition;
                    if (stateTime < 1)
                    {
                        PinkSword[0].Translation = eyePos + new Vector3(-80 + 80 * (float)Math.Cos(stateTime * Math.PI / 2), -80 + 80 * (float)Math.Sin(stateTime * Math.PI / 2), -2);
                        var sp0 = PinkSword[0].GetComponent<SpriteRendererComponent>();
                        Vector4 color = sp0.Color;
                        color.W = stateTime;
                        sp0.Color = color;

                        PinkSword[1].Translation = eyePos + new Vector3(80 - 80 * (float)Math.Cos(stateTime * Math.PI / 2), -80 + 80 * (float)Math.Sin(stateTime * Math.PI / 2), -2);
                        PinkSword[1].GetComponent<SpriteRendererComponent>().Color = color;
                    }

                    else if (stateTime < 2)
                    {
                        PinkSword[0].Rotation = new Vector3(0, 0, (1 - stateTime) * (float)Math.PI / 2);
                        PinkSword[1].Rotation = new Vector3(0, 0, (stateTime - 1) * (float)Math.PI / 2);
                    }

                    else if (stateTime < 4)
                    {
                        PinkSword[0].Translation = eyePos + new Vector3(-40 * stateTime * (float)Math.Cos(stateTime * Math.PI), -40 * stateTime * (float)Math.Sin(stateTime * Math.PI), -2);
                        PinkSword[1].Translation = eyePos + new Vector3(40 * stateTime * (float)Math.Cos(stateTime * Math.PI), 40 * stateTime * (float)Math.Sin(stateTime * Math.PI), -2);

                        PinkSword[0].Rotation = new Vector3(0, 0, (stateTime - 0.5f) * (float)Math.PI);
                        PinkSword[1].Rotation = new Vector3(0, 0, (stateTime + 0.5f) * (float)Math.PI);
                    }

                    else if (stateTime < 6)
                    {
                        PinkSword[0].Translation = eyePos + new Vector3(-160 * (float)Math.Cos(stateTime * Math.PI), -160 * (float)Math.Sin(stateTime * Math.PI), -2);
                        PinkSword[1].Translation = eyePos + new Vector3(160 * (float)Math.Cos(stateTime * Math.PI), 160 * (float)Math.Sin(stateTime * Math.PI), -2);

                        PinkSword[0].Rotation = new Vector3(0, 0, (stateTime - 0.5f) * (float)Math.PI);
                        PinkSword[1].Rotation = new Vector3(0, 0, (stateTime + 0.5f) * (float)Math.PI);
                    }

                    else if (stateTime < 8)
                    {
                        PinkSword[0].Translation = eyePos + new Vector3(40 * (stateTime - 10) * (float)Math.Cos(stateTime * Math.PI), 40 * (stateTime - 10) * (float)Math.Sin(stateTime * Math.PI), -2);
                        PinkSword[1].Translation = eyePos + new Vector3(40 * (10 - stateTime) * (float)Math.Cos(stateTime * Math.PI), 40 * (10 - stateTime) * (float)Math.Sin(stateTime * Math.PI), -2);

                        PinkSword[0].Rotation = new Vector3(0, 0, (stateTime - 0.5f) * (float)Math.PI);
                        PinkSword[1].Rotation = new Vector3(0, 0, (stateTime + 0.5f) * (float)Math.PI);
                    }

                    else if (stateTime < 9)
                    {
                        var sp0 = PinkSword[0].GetComponent<SpriteRendererComponent>();
                        Vector4 color = sp0.Color;
                        color.W = 9 - stateTime;
                        sp0.Color = color;

                        PinkSword[1].GetComponent<SpriteRendererComponent>().Color = color;
                    }

                    else
                    {
                        PinkSword[0].Pause();
                        PinkSword[1].Pause();
                        bossState = BossState.Attack;
                    }

                    break;

                case BossState.Bullet:
                    if (stateTime > 10)
                    {
                        bossState = BossState.Attack;
                        BulletGeneratorL.Pause();
                        BulletGeneratorR.Pause();
                    }
                    break;

                case BossState.Laser:
                    if (stateTime < 3)
                    {
                    }

                    else if(stateTime < 4) {
                        Vector3 rot0 = Lasers[0].Rotation;
                        rot0.Z = -(float)Math.PI * (0.75f + (1f / 6) * (stateTime - 3));

                        Lasers[0].Rotation = rot0;

                        Vector3 rot1 = Lasers[1].Rotation;
                        rot1.Z = -(float)Math.PI * (0.75f - (1f / 6) + (1f / 6) * (stateTime - 3));

                        Lasers[1].Rotation = rot1;

                        Vector3 rot2 = Lasers[2].Rotation;
                        rot2.Z = -(float)Math.PI * (0.25f + (1f / 6) - (1f / 6) * (stateTime - 3));

                        Lasers[2].Rotation = rot2;

                        Vector3 rot3 = Lasers[3].Rotation;
                        rot3.Z = -(float)Math.PI * (0.25f - (1f / 6) * (stateTime - 3));

                        Lasers[3].Rotation = rot3;
                    }

                    else if (stateTime < 8)
                    {
                        Vector3 rot0 = Lasers[0].Rotation;
                        rot0.Z = -(float)Math.PI * (0.75f + (1f / 6f) - (1f / 6) * (stateTime - 4));

                        Lasers[0].Rotation = rot0;

                        Vector3 rot1 = Lasers[1].Rotation;
                        rot1.Z = -(float)Math.PI * (0.75f - (1f / 6) * (stateTime - 4));

                        Lasers[1].Rotation = rot1;

                        Vector3 rot2 = Lasers[2].Rotation;
                        rot2.Z = -(float)Math.PI * (0.25f + (1f / 6) * (stateTime - 4));

                        Lasers[2].Rotation = rot2;

                        Vector3 rot3 = Lasers[3].Rotation;
                        rot3.Z = -(float)Math.PI * (0.25f - (1f / 6f) + (1f / 6) * (stateTime - 4));

                        Lasers[3].Rotation = rot3;
                    }

                    else if (stateTime < 11)
                    {
                        Vector3 rot0 = Lasers[0].Rotation;
                        rot0.Z = -(float)Math.PI * (0.25f + (1f / 6) * (stateTime - 8));

                        Lasers[0].Rotation = rot0;

                        Vector3 rot1 = Lasers[1].Rotation;
                        rot1.Z = -(float)Math.PI * (0.25f - (1f / 6) + (1f / 6) * (stateTime - 8));

                        Lasers[1].Rotation = rot1;

                        Vector3 rot2 = Lasers[2].Rotation;
                        rot2.Z = -(float)Math.PI * (0.75f + (1f / 6) - (1f / 6) * (stateTime - 8));

                        Lasers[2].Rotation = rot2;

                        Vector3 rot3 = Lasers[3].Rotation;
                        rot3.Z = -(float)Math.PI * (0.75f - (1f / 6) * (stateTime - 8));

                        Lasers[3].Rotation = rot3;
                    }

                    if (stateTime > 13)
                    {
                        for (int i = 0; i < Lasers.Length; i++)
                        {
                            Lasers[i].Pause();
                        }

                        bossState = BossState.Attack;
                    }
                    break;

                case BossState.Attack2Idle:
                    if (stateTime < 1)
                    {
                        Vector3 trans = Translation;
                        trans.Y = -312;
                        Translation = Vector3.Lerp(Translation, trans, ts / (1 - stateTime));

                        if (hp > 0)
                        {
                            for (int i = 0; i < 4; i++)
                            {
                                for (int j = 0; j < 23; j++)
                                {
                                    if (i > 1 && (j == 20 || j == 21))
                                        continue;

                                    if (chainSp[i, j] != null)
                                    {
                                        chainSp[i, j].Emission = new Vector4(1, 0.33333f, 0.1060f, stateTime);
                                    }
                                }
                            }

                            for (int i = 0; i < 4; i++)
                            {
                                var dj = chainBase[i].GetComponent<DistanceJointComponent>();
                                float len = dj.MinLength;

                                dj.MinLength = -len * ts / (1 - stateTime) + len;
                            }
                        }
                    }
                    else
                    {
                        if (hp > 0)
                            bossState = BossState.Idle;
                        else
                            bossState = BossState.OutBurst;
                    }
                    break;

                case BossState.Idle:
                    if (stateTime < 2f && stateTime + ts > 2f)
                    {
                        warlockLightL.Pause();
                        warlockLightR.Pause();
                    }

                    if (warlockR.GetComponent<StateComponent>().State == EntityState.Pause && warlockL.GetComponent<StateComponent>().State == EntityState.Pause)
                        bossState = BossState.Idle2Charge;
                    break;

                case BossState.Idle2Charge:
                    if (stateTime > 1)
                        bossState = BossState.Charge;
                    else
                    {
                        WingL.Rotation = new Vector3(0, 0, stateTime * (float)Math.PI / 9);
                        WingR.Rotation = new Vector3(0, 0, -stateTime * (float)Math.PI / 9);

                        Translation = Vector3.Lerp(Translation, new Vector3(96, -280, 2.0f), ts / (1 - stateTime));
                    }
                    break;

                case BossState.OutBurst:
                    if (stateTime >1.5f)
                        bossState = BossState.Falling;
                    break;

                case BossState.Falling:
                    if (stateTime < 3)
                        Translation = Vector3.Lerp(Translation, new Vector3(96, -604, 2.0f), ts / (3 - stateTime));
                    else
                        bossState = BossState.ChainDissolve;
                    break;

                case BossState.ChainDissolve:
                    if (stateTime > 1 && stateTime < 7)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            for (int j = 0; j < 23; j++)
                            {
                                if (chainSp[i, j] != null)
                                    chainSp[i, j].SetFloat(0, 9 - 50 * (stateTime -1));
                            }
                        }
                        
                    }
                    if (stateTime > 7)
                        bossState = BossState.Miracle;
                    break;

                case BossState.Miracle:
                    if (stateTime < 3)
                    { 
                    }

                    else if (stateTime < 8)
                        Translation = Vector3.Lerp(Translation, new Vector3(96, -280, 2.0f), ts / (8f - stateTime));
                    else
                        bossState = BossState.Cleanse;
                    break;

                case BossState.Cleanse:
                    if (stateTime < 7)
                    {
                        smallPS.Position = new Vector2(-1 + 60 * stateTime, -90);
                        BodySp.SetFloat(0, 60 * stateTime);
                        WingLSp.SetFloat(0, 60 * stateTime);
                        WingRSp.SetFloat(0, 60 * stateTime);
                        eyeSp.SetFloat(0, 60 * stateTime);
                        pupilSp.SetFloat(0, 60 * stateTime);
                        HolyBodySp.SetFloat(0, 60 * stateTime);
                        HolyWingLSp.SetFloat(0, 60 * stateTime);
                        HolyWingRSp.SetFloat(0, 60 * stateTime);
                    }
                    else
                        bossState = BossState.HolyCharge;
                    break;

                case BossState.HolyCharge:
                    if (stateTime < 3)
                    {
                        WingL.Rotation = new Vector3(0, 0, 20.0f * stateTime / 3f * (float)Math.PI / 180.0f);
                        WingR.Rotation = new Vector3(0, 0, -20.0f * stateTime / 3f * (float)Math.PI / 180.0f);
                    }
                    else
                        bossState = BossState.HolyEnd;
                    break;
                case BossState.HolyEnd:
                    if (stateTime < 1)
                    {
                        WingL.Rotation = new Vector3(0, 0, (1 - stateTime) * (float)Math.PI / 9.0f);
                        WingR.Rotation = new Vector3(0, 0, (stateTime - 1) * (float)Math.PI / 9.0f);
                        Vector3 hTrans = Translation;
                        hTrans.Y += 50 * ts;
                        Translation = hTrans;
                    }
                    break;

            }
            UpdateEye();

            if (bossState != BossState.Chain && bossState != BossState.Sleep && bossState < (BossState)13)
            { 
                for (int i = 0; i < 4; i++)
                {
                    chainEnd[i].Translation = Translation + chainOffsets[i];
                }
            }
            if (bossState == BossState.Sword || bossState == BossState.Laser || bossState == BossState.Bullet || bossState == BossState.Attack)
            {
                WingL.Rotation = new Vector3(0, 0, (float)(Math.PI / 36 * Math.Sin(stateTime * Math.PI)));
                WingR.Rotation = new Vector3(0, 0, -(float)(Math.PI / 36 * Math.Sin(stateTime * Math.PI)));
                Vector3 trans = Translation;
                trans.Y = -280 + 5 * (float)Math.Sin(stateTime * Math.PI);
                Translation = trans;
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
            pupil.Translation = diff * 2.0f;
        }

        public override void OnAttacked(Weapon opp)
        {
            attackedBefore = 0.0f;
            blood.State = (uint)ParticleSystem.ParticleState.NORMAL;

            if (opp.WorldPosition.X > Translation.X)
            {
                blood.VelocityBegin = new Vector2(100, 0);
                blood.VelocityEnd = new Vector2(100, -50);
            }
            else
            {
                blood.VelocityBegin = new Vector2(-100, 0);
                blood.VelocityEnd = new Vector2(-100, -50);
            }     
        }

        public override void ReceiveDamage(float damage)
        {
            HP -= 10;
            if (hp < (5 - bossPhase) * 20 || hp <= 0)
            {
                if (hp <= 0)
                {
                    Timer.SetTimeScale(0.5f);
                    BossHpBar.Pause();
                    FindEntityByName("MainCamera").GetComponent<AudioSource>().Stop();
                }

                Invisible = true;
                bossState = BossState.Attack2Idle;
                bossPhase += 1;

                PinkSword[0].Pause();
                PinkSword[1].Pause();
                BulletGeneratorL.Pause();
                BulletGeneratorR.Pause();
                _audioSource.Stop();

                for (int i = 0; i < Lasers.Length; i++)
                {
                    Lasers[i].Pause();
                }

                halo.GetComponent<ParticleSystem>().State = (uint)ParticleSystem.ParticleState.NORMAL;

                summonedEyeR.Pause();
                summonedEyeL.Pause();
            }
        }
    }
}
