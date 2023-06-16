using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Portal : Entity
    {
        private string LoadingScenePath = "Resources\\assets\\Scenes\\LoadingScene.hazel";
        public string nextScene;
        public Vector3 disiredPosition;
        protected Entity MainCamera;
        private Entity player;

        bool sceneChange = false;
        float stateTime = 0f;
        internal virtual void OnTriggerEnter(ref Collision2D collsion)
        {
            player = new Entity(collsion.entityID);
            sceneChange = true;
        }

        void OnCreate()
        {
            MainCamera = FindEntityByName("MainCamera");
        }

        internal virtual void OnUpdate(float ts)
        {
            if (sceneChange)
            {
                if (stateTime < 1F)
                    MainCamera.GetComponent<CameraComponent>().FadeColor = new Vector4(0, 0, 0, stateTime);

                else {
                    player.DontDestroy(ref disiredPosition);
                    SceneManager.ReserveNextScene(nextScene);
                    SceneManager.LoadScene(LoadingScenePath);
                }
                stateTime += ts;
            }
        }
    }
}
