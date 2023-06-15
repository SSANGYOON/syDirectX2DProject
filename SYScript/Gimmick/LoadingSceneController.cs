using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class LoadingSceneController : Entity
    {
        public float accTime;
        void OnCreate()
        {
            var sa = GetComponent<SpriteAnimatorComponent>();
            sa.Play("Loading");

            accTime = 0.0f;
        }

        void OnUpdate(float ts)
        {
            accTime += ts;

            if(accTime > 5.0f) {
                SceneManager.LoadReservedScene();
            }
            
        }
    }
}
