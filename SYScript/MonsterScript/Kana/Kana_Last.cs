using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Kana_Last : Entity
    {
        private Vector2 velocity;
        private float _stateTime = 0f;

        void OnCreate()
        {
        }

        void OnActivated()
        {
            velocity.X = (224 - Translation.X) / 2;
            velocity.Y = (400 - 47 - Translation.Y) / 2;
            Camera cam =FindEntityByName("MainCamera").As<Camera>();
            cam.Cinematic = true;
            cam.Following = this;
        }

        void OnUpdate(float ts)
        {
            if (_stateTime < 2)
            {
                velocity.Y -= 200 * ts;
                Vector3 trans = Translation;
                trans.X += velocity.X * ts;
                trans.Y += velocity.Y * ts;
                Translation = trans;
                if (_stateTime + ts > 2)
                {
                    Timer.SetTimeScale(1);
                    GetComponent<SpriteAnimatorComponent>().Play("Chained_Kana");
                }
            }

            if (_stateTime < 4 && _stateTime + ts >4)
                FindEntityByName("Altar_Particle").Activate();
            if (_stateTime > 4)
            {
                Vector3 trans = Translation;
                trans.Y -= 25 * ts;
                Translation = trans;
            }

            if (_stateTime > 7)
            {
                FindEntityByName("Altar_Particle").Pause();
                Pause();

                Camera cam = FindEntityByName("MainCamera").As<Camera>();
                cam.Cinematic = false;
                cam.Following = null;
            }

            _stateTime += ts;
        }
    }
}
