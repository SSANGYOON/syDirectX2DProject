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
        float accTime;
        void OnCreate()
        {
            var sa = GetComponent<SpriteAnimatorComponent>();
            sa.Play("Loading");

            accTime = 0.0f;
        }

        void OnUpdate(float ts)
        {
            accTime += ts;
            if (accTime > 4f && accTime < 5.0f)
                FindEntityByName("Camera").GetComponent<CameraComponent>().FadeColor = new Vector4(0, 0, 0, accTime - 4);
            else if (accTime > 5.0f)
                SceneManager.LoadReservedScene();       
        }
    }
}
