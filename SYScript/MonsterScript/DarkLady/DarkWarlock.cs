using SY;
using System;
using static Sandbox.BossController;


namespace Sandbox
{
    public class DarkWarlock : Character
    {
        public enum WarlockState
        {
            Summon,
            Idle,
            Attack,
            Heal,
            Damaged,
            Dead,
            SummonBack
        }

        WarlockState _state;
        SpriteAnimatorComponent animator;

        public Entity thunderBallPref;

        Entity[] thunderBall;

        public Entity particleSpherePref;
        public Entity lineParticlePref;

        Entity particleSphere;
        Entity lineParticleFromThis;
        Entity lineParticleFromSphere;
        Entity boss;
        Entity player;
        Entity summonParticle;

        public Entity debris0Pref;
        public Entity debris1Pref;
        public Entity debris2Pref;

        Entity debris0;
        Entity debris1;
        Entity debris2;

        bool _healing = false;
        bool _approaching = false;

        float HealRange = 250f;
        double thunderBallAngle = 0;
        double thunderBallAngularVelocity = Math.PI;
        uint HitOnHealing = 0;

        bool Healing
        {
            set { _healing = value;
                if (_healing)
                {
                    particleSphere.Activate();
                    lineParticleFromSphere.Activate();
                    lineParticleFromThis.Activate();
                }
                else
                {
                    particleSphere.Pause();
                    lineParticleFromThis.Pause();
                    lineParticleFromSphere.Pause();
                    HitOnHealing = 0;
                }
            }
            get { return _healing; }
        }
        float thunderBallRadius = 20f;
        float thunderBallCoolTime = 0f;
        float thunderBallLifeRemain = 0f;

        public WarlockState State
        {
            set
            {
                var prevState = _state;
                _state = value;
                stateTime = 0.0f;               
                switch (_state)
                {
                    case WarlockState.Summon:
                        rigidBody.Enable = false;

                        debris0.Pause();
                        debris1.Pause();
                        debris2.Pause();
                        break;

                    case WarlockState.Idle:
                        rigidBody.Enable = true;
                        if(prevState == WarlockState.Attack || prevState == WarlockState.Heal)
                            animator.Play("WARLOCKATTACK2IDLE");
                        else
                            animator.Play("WARLOCKIDLE");
                        break;

                    case WarlockState.Attack:
                        animator.Play("WARLOCKATTACK");
                        break;

                    case WarlockState.Heal:
                        
                        animator.Play("WARLOCKATTACK");

                        break;

                    case WarlockState.Damaged:
                        animator.Play("WARLOCKDAMAGED");
                        break;

                    case WarlockState.Dead:
                        animator.Play("WARLOCKDEAD");
                        break;

                    case WarlockState.SummonBack:
                        summonParticle.Activate();
                        break;

                    default:
                        break;
                }
            }
            get { return _state; }
        }

        void OnCreate()
        {
            rigidBody = GetComponent<Rigidbody2DComponent>();
            animator = GetComponent<SpriteAnimatorComponent>();           
            hitBox = GetComponent<BoxCollider2DComponent>();

            boss = FindEntityByName("DarkLady");
            player = FindEntityByName("Player");
            particleSphere = particleSpherePref.Instantiate(new Vector3(),0);
            lineParticleFromThis = lineParticlePref.Instantiate(new Vector3(), 0);
            lineParticleFromSphere = lineParticlePref.Instantiate(new Vector3(), 0);

            Healing = false;
            thunderBall = new Entity[3];

            for (int i = 0; i < 3; i++)
            {
                thunderBall[i] = thunderBallPref.Instantiate(new Vector3(), ID);
                thunderBall[i].Pause();
            }

                debris0 = debris0Pref.Instantiate(new Vector3());
            debris1 = debris1Pref.Instantiate(new Vector3());
            debris2 = debris2Pref.Instantiate(new Vector3());

            debris0.Pause();
            debris1.Pause();
            debris2.Pause();
            summonParticle = GetChild("wParticle");
        }

        void OnActivated()
        {
            State = WarlockState.Summon;
            animator.Play("WARLOCKIDLE");
            hp = 6;

            thunderBallRadius = 20f;
            thunderBallCoolTime = 0f;
            thunderBallLifeRemain = 0f;
        }

        void OnUpdate(float ts)
        {
            switch (State)
            {
                case WarlockState.Summon:
                    if (stateTime < 2f)
                    {
                        Vector3 trans = Translation;
                        trans.Y = -128f;
                        Translation = Vector3.Lerp(Translation, trans, ts / (2f - stateTime));
                    }
                    else
                        State = WarlockState.Idle;
                    break;

                case WarlockState.Idle:
                    if (!Healing && HealRange >= Math.Abs(boss.Translation.X - Translation.X))
                    {
                        State = WarlockState.Heal;

                        if (Flip)
                            particleSphere.Translation = new Vector3(24, 50, 0) + Translation;
                        else
                            particleSphere.Translation = new Vector3(-24, 50, 0) + Translation;

                        lineParticleFromSphere.Translation = particleSphere.Translation;

                        var ps = lineParticleFromSphere.GetComponent<ParticleSystem>();

                        Vector2 diff = (boss.Translation.XY + new Vector2(0, 155) - particleSphere.Translation.XY);
                        float length = (diff).Length();

                        ps.LifeTime = length / 50;
                        ps.AliveZone = new Vector2(length, 10);
                        lineParticleFromSphere.Rotation = new Vector3(0, 0, (float)Math.Atan2(diff.Y, diff.X));
                    }
                    else if (thunderBallCoolTime <= 0f && thunderBallLifeRemain <= 0f)
                    {
                        for (int i = 0; i < 3; i++)
                        {
                            thunderBall[i].Activate();

                            double angle = Math.PI / 2 + 2 * Math.PI / 3 * i;
                            thunderBall[i].Translation = new Vector3((float)Math.Cos(angle), (float)Math.Sin(angle), 0);
                        }

                        thunderBallAngle = 0f;
                        thunderBallLifeRemain = 10f;
                        State = WarlockState.Attack;
                    }
                    {
                        float bossDiff = boss.Translation.X - Translation.X;
                        float diff = player.Translation.X - Translation.X;

                        if (Math.Abs(bossDiff) > HealRange)
                            _approaching = true;

                        else if (Math.Abs(bossDiff) < 100f)
                            _approaching = false;

                        if (_approaching)
                        {
                            if (bossDiff > 0)
                            {
                                Vector3 trans = Translation;
                                trans.X += 50f * ts;
                                Translation = trans;
                            }
                            else
                            {
                                Vector3 trans = Translation;
                                trans.X -= 50f * ts;
                                Translation = trans;
                            }
                        }

                        else if (Math.Abs(diff) < 50)
                        {
                            if (diff < 0)
                            {
                                Vector3 trans = Translation;
                                trans.X += 50f * ts;
                                Translation = trans;
                            }
                            else
                            {
                                Vector3 trans = Translation;
                                trans.X -= 50f * ts;
                                Translation = trans;
                            }
                        }

                        if (HealRange <= Math.Abs(boss.Translation.X - Translation.X) && Healing)
                            Healing = false;
                    }
                    break;

                case WarlockState.Heal:
                    if (stateTime > 1)
                    {
                        Healing = true;
                        State = WarlockState.Idle;
                    }
                    break;

                case WarlockState.Attack:
                    if (stateTime > 1)
                        State = WarlockState.Idle;
                    break;

                case WarlockState.Damaged:
                    if (stateTime > 0.5)
                        State = WarlockState.Idle;

                    if (hp <= 0)
                    {
                        float flip = Flip ? -1f : 1f;

                        debris0.Activate();
                        Vector3 trans = Translation;
                        debris0.Translation = trans + new Vector3(0f, 18.5f, 0f);
                        var rb0 = debris0.GetComponent<Rigidbody2DComponent>();
                        rb0.LinearVelocity = new Vector2(flip * 0.707F, 0.707F) * 60;
                        rb0.AngularVelocity = 2 * flip * (float)Math.PI;

                        debris1.Activate();
                        debris1.Translation = trans + new Vector3(-7f * flip, 16.5f, 0f);
                        var rb1 = debris1.GetComponent<Rigidbody2DComponent>();
                        rb1.LinearVelocity = new Vector2(flip * -0.5F, 0.85F) * 60;
                        rb1.AngularVelocity = 2 * flip * (float)Math.PI;

                        debris2.Activate();
                        debris2.Translation = trans + new Vector3(-9.5f * flip, -11.5f, 0f);
                        var rb2 = debris2.GetComponent<Rigidbody2DComponent>();
                        rb2.LinearVelocity = new Vector2(-1f * flip, 0) * 60;
                        rb2.AngularVelocity = 2 * flip * (float)Math.PI;

                        State = WarlockState.Dead;
                        Healing = false;
                        for (int i = 0; i < 3; i++)
                            thunderBall[i].Pause();
                    }
                    break;

                case WarlockState.Dead:
                    if (stateTime > 0.5)
                    {
                        rigidBody.Enable = false;
                        State = WarlockState.SummonBack;
                    }
                    break;
                case WarlockState.SummonBack:
                    if (stateTime < 2f)
                    {
                        Vector3 trans = Translation;
                        trans.Y = -208;
                        Translation = Vector3.Lerp(Translation, trans, ts / (2f - stateTime));
                        summonParticle.Translation = new Vector3(0, -168 - Translation.Y, 0);
                    }
                    else {
                        summonParticle.Pause();
                        Pause();
                    }
                    break;
                default:
                    break;
            }

            if (boss.Translation.X > Translation.X)
                Flip = true;
            else
                Flip = false;

            if (Healing)
            {
                lineParticleFromThis.Translation = Translation;
                var ps = lineParticleFromThis.GetComponent<ParticleSystem>();

                Vector2 diff = (particleSphere.Translation.XY - Translation.XY);
                float length = (particleSphere.Translation.XY - Translation.XY).Length();

                ps.LifeTime = length / 50;
                ps.AliveZone = new Vector2(length, 10);
                lineParticleFromThis.Rotation = new Vector3(0,0,(float)Math.Atan2(diff.Y , diff.X));
            }

            if (thunderBallLifeRemain > 0)
            {
                thunderBallAngle += ts * thunderBallAngularVelocity;

                for (int i = 0; i < 3; i++)
                {
                    double angle = Math.PI / 2 + thunderBallAngle + 2 * Math.PI / 3 * i;
                    thunderBall[i].Translation = new Vector3((float)Math.Cos(angle) * thunderBallRadius, (float)Math.Sin(angle) * thunderBallRadius, 0);
                }
                thunderBallLifeRemain -= ts;

                if (thunderBallLifeRemain < 0)
                {
                    thunderBallCoolTime = 5f;
                    for (int i = 0; i < 3; i++)
                    {
                        thunderBall[i].Pause();
                    }
                }
            }
            else if(thunderBallCoolTime > 0)
                thunderBallCoolTime -= ts;

            stateTime += ts;
        }

        public override void OnAttacked(Weapon opp)
        {
            if (opp.WorldPosition.X > Translation.X)
                rigidBody.ApplyLinearImpulse(new Vector2(-500 * hitBox.Size.Y * hitBox.Size.X, 250 * hitBox.Size.Y * hitBox.Size.X), true);
            else
                rigidBody.ApplyLinearImpulse(new Vector2(500 * hitBox.Size.Y * hitBox.Size.X, 250 * hitBox.Size.Y * hitBox.Size.X), true);

            State = WarlockState.Damaged;

            if (Healing)
            {
                HitOnHealing++;
                if (HitOnHealing > 2)
                {
                    Healing = false;
                    HitOnHealing = 0;
                }
            }
        }

        public override void ReceiveDamage(float damage)
        {
            hp -= 3;
        }
    }
}
