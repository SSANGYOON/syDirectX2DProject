using Sandbox;
using SY;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class BossController : Entity
    {   
        private Entity[] chain;
        private Entity[] cp;

        private Entity player;
        private Entity halo;
        private Entity eye;

        private Entity WingL;
        private Entity WingR;
        private Entity Circlet;

        private Entity Body;
        private Entity Hair;

        private Entity BossSummoner;

        private Vector3[] chainOffsets;

        public enum BossState
        { 
            Sleep,
            Chain,
            Appearing,
            Charge,
            IDLE,
        }

        private BossState _state;

        public BossState bossState
        {
            set{
                if(value == BossState.Chain)
                    halo.Activate();

                _state = value;
                stateTime = 0.0f;
            }
            get{ return _state;  }
        }

        float stateTime = 0.0f;
        float chainMoveTime = 2.5f;
        float chainInterval = 1.0f;

        void OnCreate()
        {
            chain = new Entity[4];
            cp = new Entity[4];
            chainOffsets = new Vector3[4];
            for (int i=0;i< 4;i++)
            {
                chain[i] = FindEntityByName("Chain" +(i+1) +"-end");
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

            Circlet = FindEntityByName("DarkLadyCirclelet");

            Body = FindEntityByName("DarkLadyBody");
            Hair = FindEntityByName("DarkLadyHair");

            var bodyanim = Body.GetComponent<SpriteAnimatorComponent>();
            bodyanim.Play("IDLE");

            var hairAnim  = Hair.GetComponent<SpriteAnimatorComponent>();
            hairAnim.Play("HAIR");
        }

        void OnUpdate(float ts)
        {
            switch (bossState)
            { 
                case BossState.Chain:
                    if(stateTime > 5.5f) {
                        bossState = BossState.Appearing;
                        var particle = halo.GetComponent<ParticleSystem>();
                        particle.State = (uint)ParticleSystem.ParticleState.NORMAL;
                    }

                    for (int i = 0; i < 4; i++)
                    {
                        if (stateTime + ts < chainMoveTime + chainInterval * i && stateTime + ts > chainInterval * i)
                        {
                            if (stateTime <= chainInterval * i)
                                cp[i].Activate();

                            chain[i].Translation = Vector3.Lerp(chain[i].Translation, Translation + chainOffsets[i], ts / (chainMoveTime + chainInterval * i - stateTime));
                        }
                    }
                    break;
                case BossState.Appearing:
                    if (stateTime > 4.5)
                    {
                        bossState = BossState.Charge;
                        var particle = halo.GetComponent<ParticleSystem>();
                        particle.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                    }
                    else
                    {
                        WingL.Rotation = new Vector3(0, 0, 20.0f * stateTime / 4.5f * (float)Math.PI / 180.0f);
                        WingR.Rotation = new Vector3(0, 0, -20.0f * stateTime / 4.5f * (float)Math.PI / 180.0f);

                        Translation = Vector3.Lerp(Translation, new Vector3(96, -280, 2.0f), ts / (4.5f - stateTime));
                        for (int i = 0; i < 4; i++)
                        {
                            chain[i].Translation = Translation + chainOffsets[i];
                        }
                    }
                    break;
                case BossState.Charge:
                    if (stateTime > 1.0f)
                    {
                        for (int i = 1; i <= 4; i++)
                        {
                            halo.Pause();
                            string name = "Chain" + i + "-Base";
                            Entity chain = FindEntityByName(name);

                            if (chain != null)
                            {
                                var dj = chain.GetComponent<DistanceJointComponent>();
                                dj.MinLength = dj.CurrentLength * 1.2f;
                            }
                        }
                        var ps = Circlet.GetComponent<ParticleSystem>();
                        ps.State = (uint)ParticleSystem.ParticleState.NORMAL;
                        bossState = BossState.IDLE;

                        FindEntityByName("SummonedEyeR").Activate();
                        FindEntityByName("SummonedEyeL").Activate();

                        for (int i = 0; i < 3; i++)
                        {
                            string name = "BrightWingL" + i;
                            Entity lwing = FindEntityByName(name);
                            lwing.Activate();

                            name = "BrightWingR" + i;
                            Entity rwing = FindEntityByName(name);
                            rwing.Activate();
                        }

                        Entity haloParticle = FindEntityByName("HaloParticle");
                        var haloPs = haloParticle.GetComponent<ParticleSystem>();
                        haloPs.State = (uint)ParticleSystem.ParticleState.NORMAL;

                    }
                    else
                    {
                        WingL.Rotation = new Vector3(0, 0, (20.0f - 20.0f * stateTime) * (float)Math.PI / 180.0f);
                        WingR.Rotation = new Vector3(0, 0, (-20.0f + 20.0f * stateTime) * (float)Math.PI / 180.0f);
                    }
                    break;

                case BossState.IDLE:
                    {
                        WingL.Rotation = new Vector3(0, 0, -5 * (float)Math.Sin(stateTime * Math.PI) * (float)Math.PI / 180.0f);
                        WingR.Rotation = new Vector3(0, 0, 5 * (float)Math.Sin(stateTime * Math.PI) * (float)Math.PI / 180.0f);


                        Vector3 vector3 = new Vector3(96,-280 + 5 * (float)Math.Sin(stateTime * Math.PI), 2.0f);
                        Translation = vector3;

                        for (int i = 0; i < 4; i++)
                        {
                            chain[i].Translation = Translation + chainOffsets[i];
                        }
                    }
                    break;
            }
            UpdateEye();
            stateTime += ts;
        }

        void UpdateEye() {

            Vector3 diff = player.WorldPosition - eye.WorldPosition;
            diff.Normalize();

            Entity pupil = FindEntityByName("DarkLadyPupil");

            pupil.Translation = diff * 2.0f;
        }
    }
}
