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

        private  Entity m_Player;

        void OnCreate()
        {
            m_Player = FindEntityByName("Player");
        }

        void OnUpdate(float ts)
        {
            if (m_Player == null)
                return;

            Vector3 playerPos = m_Player.Translation;
            Vector2 targetPos = playerPos.XY;

            Vector2 cameraSize = GetComponent<CameraComponent>().OrthographicSize;

            

            Vector2 minPos = cameraSize / 2.0f - CameraRange / 2.0f;
            Vector2 maxPos = CameraRange / 2.0f - cameraSize / 2.0f;
            Vector2 nextPos = targetPos.clamp(minPos, maxPos);

            Translation = new Vector3(nextPos, playerPos .Z - DistanceFromPlayer);
        }

    }
}