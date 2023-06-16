using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using SY;

namespace Sandbox
{
    public class Camera : Entity
    {
        public  float DistanceFromPlayer = 5.0f;
        public Vector2 CameraRange = new Vector2(640, 360);

        private Entity m_Player;
        internal Entity Following;
        private bool onCinematic = false;
        private bool enterScene = false;
        public bool Cinematic
        { 
            get { return onCinematic; }
            set { onCinematic = value; stateTime = 0f; 
                if(!value)
                    GetComponent<CameraComponent>().ScissorRect = new Vector4(0, 0, 1, 1);
            }
        }

        private float stateTime;
        void OnCreate()
        {
            m_Player = FindEntityByName("Player");
            enterScene = true;
        }

        void OnUpdate(float ts)
        {
            if (m_Player == null)
                return;
            if (Cinematic)
                if (stateTime < 1f)
                    GetComponent<CameraComponent>().ScissorRect = new Vector4(0, stateTime * 0.12f, 1, 1 - stateTime * 0.24f);

            Vector2 targetPos;
            var cameraComp = GetComponent<CameraComponent>();

            if (enterScene && stateTime < 1)
                cameraComp.FadeColor = new Vector4(0, 0, 0, 1 - stateTime);
            if (stateTime > 1)
            {
                enterScene = false;
                cameraComp.FadeColor = new Vector4(0, 0, 0, 0);
            }

            if (Cinematic)
                targetPos = Following.Translation.XY;
            else
                targetPos = m_Player.Translation.XY;

            Vector2 cameraSize = GetComponent<CameraComponent>().OrthographicSize;

            Vector2 minPos = cameraSize / 2.0f - CameraRange / 2.0f;
            Vector2 maxPos = CameraRange / 2.0f - cameraSize / 2.0f;
            Vector2 nextPos = targetPos.clamp(minPos, maxPos);

            Vector2 diff = nextPos - Translation.XY;

            if (diff.LengthSquared() > 1)
            {
                diff.Normalize();
                Translation = new Vector3(Translation.X + diff.X * 150f * ts, Translation.Y + diff.Y * 150f * ts, Translation.Z);
            }

            stateTime += ts;
        }

    }
}