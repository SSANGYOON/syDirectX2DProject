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
        private Vector4 white;
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
            private set { _state = value; stateTime = 0.0f;}
            get { return _state; }
        }

        void OnCreate()
        {
            Body = GetChild("seBody");
            Eye = GetChild("seEye");

            pupil = Eye.GetChild("sePupil");
            
            sword = swordPref.Instantiate(Translation, 0).As<BossSword>();
            sword.Active = false;
            sword.Pause();

            sa = Eye.GetComponent<SpriteAnimatorComponent>();
            white = new Vector4(1, 1, 1, 1);
            player = FindEntityByName("Player");           
        }

        void OnActivated()
        {
            State = eyeState.Summon;

            var ps = Body.GetComponent<ParticleSystem>();
            ps.Position = new Vector2(0, -50.0f + (stateTime / 2.0f) * 100.0f);
            ps.State = (uint)ParticleSystem.ParticleState.NORMAL;

            var bsr = Body.GetComponent<SpriteRendererComponent>();

            white.W = 0;
            bsr.Color = white;

            var esr = Eye.GetComponent<SpriteRendererComponent>();
            esr.Color = white;

            Eye.GetChild("seWhite").Pause();
            pupil.Pause();
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
                        white.W = 1;
                        bsr.Color = white;

                        var esr = Eye.GetComponent<SpriteRendererComponent>();
                        esr.Color = white;

                        var eyeWhite = Eye.GetChild("seWhite");

                        eyeWhite.Activate();
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
                    if (stateTime > 3.0f)
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

                        var body = sword.GetChild("BossSwordBody");
                        sword.Rotation = new Vector3();
                        var sr = body.GetComponent<SpriteRendererComponent>();
                        var color = sr.Color;
                        color.W = 1;
                        sr.Color = color;

                        var Emission = sr.Emission;
                        Emission.W = 1;
                        sr.Emission = Emission;
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

                        if(stateTime + ts > 1.0f)
                            sword.Active = true;
                    }

                    else
                    {
                        if (sword.Active)
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
                        else
                            State = eyeState.Closing;
                        sa.Play("Close");
                    }
                    break;
                case eyeState.Closing:
                    if (stateTime > 1.0f)
                    {
                        State = eyeState.Idle;
                        
                        var ps = sword.GetComponent<ParticleSystem>();
                        ps.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                    }

                    else
                    {
                        var body = sword.GetChild("BossSwordBody");
                        var sr = body.GetComponent<SpriteRendererComponent>();

                        var color = sr.Color;
                        color.W = 1 - stateTime;
                        sr.Color = color;

                        var Emission = sr.Emission;
                        Emission.W = 1 - stateTime;
                        sr.Emission = Emission;
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

        void OnPaused()
        {
            sword.Pause();
        }
    }
}
