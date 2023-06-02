using SY;
using System;
using System.Collections.Generic;
using System.Diagnostics.Eventing.Reader;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class SummonedEye : Entity
    {
        public float detectionRange;

        private Entity Body;
        private Entity Eye;

        private Entity pupil;
        private Entity player;

        public Entity swordPref;
        private BossSword sword;

        private SpriteAnimatorComponent sa;

        private float targetDiff;

        private float stateTime;
        public enum eyeState
        {
            Summon,
            Idle,
            Ready,
            Targeting,
            Attack,
            Closing,
            Pause
        }

        private eyeState _state;
        public eyeState State
        {
            private set { _state = value; stateTime = 0.0f; }
            get { return _state; }
        }

        void OnCreate()
        {
            Body = GetChild("seBody");
            Eye = GetChild("seEye");

            pupil = Eye.GetChild("sePupil");
            State = eyeState.Summon;

            sa = Eye.GetComponent<SpriteAnimatorComponent>();

            player = FindEntityByName("Player");

            var ps = Body.GetComponent<ParticleSystem>();
            ps.State = (uint)ParticleSystem.ParticleState.NORMAL;
        }

        void OnUpdate(float ts)
        {
            switch (State)
            {
                case eyeState.Summon:
                    if (stateTime > 2.4f)
                    {
                        var ps = Eye.GetComponent<ParticleSystem>();
                        ps.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;

                        var bsr = Body.GetComponent<SpriteRendererComponent>();
                        bsr.Color = new Vector4(1, 1, 1, 1);

                        var esr = Eye.GetComponent<SpriteRendererComponent>();
                        esr.Color = new Vector4(1, 1, 1, 1);

                        var white = Eye.GetChild("seWhite");

                        white.Activate();
                        pupil.Activate();

                        State = eyeState.Idle;
                    }
                    else
                    {
                        if (stateTime < 2.0f)
                        {
                            var ps = Body.GetComponent<ParticleSystem>();
                            ps.Position = new Vector2(0, -50.0f + (stateTime / 2.0f) * 100.0f);

                            if (stateTime + ts > 2.0f)
                                ps.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;                          
                        }

                        if (stateTime + ts > 1.6f && stateTime < 1.6f)
                        {
                            var ps = Eye.GetComponent<ParticleSystem>();
                            ps.State = (uint)ParticleSystem.ParticleState.NORMAL;
                        }

                        if (stateTime > 1.6f)
                        {
                            var ps = Eye.GetComponent<ParticleSystem>();
                            ps.State = (uint)ParticleSystem.ParticleState.NORMAL;
                            ps.Position = new Vector2(0, -20.0f + ((stateTime - 1.6f) / 2.0f) * 100.0f);
                        }
                    }
                    break;

                case eyeState.Idle:
                    if (stateTime > 2.0f)
                        State = eyeState.Ready;

                    else
                        Body.GetComponent<ParticleSystem>().Position = new Vector2(0, -50.0f + (stateTime / 2.0f) * 100.0f);

                    break;

                case eyeState.Ready:
                    if (Math.Abs(Translation.X - player.Translation.X) < detectionRange)
                    {
                        targetDiff = player.Translation.X - Translation.X;
                        State = eyeState.Targeting;
                        sa.Play("Open");

                        sword = swordPref.Instantiate(Translation,0).As<BossSword>();
                        sword._owner = this;
                        sword.Pause();
                    }
                    break;

                case eyeState.Targeting:
                    sword.Activate();
                    if (stateTime < 0.5f)
                    {
                        if (targetDiff > 0)
                            sword.Translation = new Vector3(Translation.X + targetDiff - 96.0f, Translation.Y - (200 - stateTime * 600), sword.Translation.Z);
                        else
                            sword.Translation = new Vector3(Translation.X + targetDiff + 96.0f, Translation.Y - (200 - stateTime * 600), sword.Translation.Z);
                    }

                    else if (stateTime < 1.0f)
                    {
                        if (targetDiff > 0)
                            sword.Rotation = new Vector3(0, 0, 2.7f * (stateTime - 0.5f) * (float)Math.PI);
                        else
                            sword.Rotation = new Vector3(0, 0, -2.7f * (stateTime - 0.5f) * (float)Math.PI);
                    }

                    else if (stateTime < 1.5f)
                    {
                        if (targetDiff > 0)
                        {
                            sword.Rotation = Vector3.Lerp(sword.Rotation, new Vector3(0, 0, 0.25f * (float)Math.PI), ts / (1.5f - stateTime));
                            sword.Translation = new Vector3(sword.Translation.X, sword.Translation.Y - 200.0f * ts, sword.Translation.Z);
                        }
                        else
                        {
                            sword.Rotation = Vector3.Lerp(sword.Rotation, new Vector3(0, 0, -0.25f * (float)Math.PI), ts / (1.5f - stateTime));
                            sword.Translation = new Vector3(sword.Translation.X, sword.Translation.Y - 200.0f * ts, sword.Translation.Z);
                        }
                    }
                    break;
                case eyeState.Closing:
                    if (stateTime > 1.0f)
                    {
                        State = eyeState.Pause;

                        var ps = sword.GetComponent<ParticleSystem>();
                        ps.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                    }

                    else
                    {
                        var body = sword.GetChild("BossSwordBody");
                        var sr = body.GetComponent<SpriteRendererComponent>();

                        var color = sr.Color;
                        color.W = (1 - stateTime);
                        sr.Color = color;

                        var Emission = sr.Emission;
                        Emission.W = (1 - stateTime);
                        sr.Emission = Emission;
                    }
                    break;

                case eyeState.Pause:
                    if (stateTime > 1.0f)
                    {
                        sword.Destroy();
                        State = eyeState.Idle;
                    }
                    break;
                default:
                    break;
            }

            Vector3 trans = pupil.Translation;
            Vector2 norm = (player.WorldPosition - pupil.WorldPosition - trans).XY;
            norm.Normalize();
            pupil.Translation = new Vector3(4.0f * norm.X, 4.0f * norm.Y, trans.Z);

            stateTime += ts;
        }
        internal void OnTriggerEnter(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 0)) > 0)
            {
                var opp = new Entity(collsion.entityID);
                if (opp.Translation.Y < sword.Translation.Y -20)
                {
                    State = eyeState.Closing;
                    sa.Play("Close");
                    FindEntityByName("MainCamera").GetComponent<CameraComponent>().AddOscilation(5.0f);

                    if (player.As<Player>().Grounded) {

                        var rb = player.GetComponent<Rigidbody2DComponent>();
                        var bc = player.GetComponent<BoxCollider2DComponent>();
                        rb.ApplyLinearImpulse(new Vector2(0, 500 * bc.Size.Y * bc.Size.X), true);
                    }
                }
            }
        }
    }
}
