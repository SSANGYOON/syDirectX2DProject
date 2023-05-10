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
        void OnTriggerEnter(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 1)) > 0)
            {
                SceneManager.ReserveNextScene(nextScene);
                SceneManager.LoadScene(LoadingScenePath);
            }
        }
    }
}
