using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Resources;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class TitleSceneController : Entity
    {
        void OnCreate()
        {
            Entity player = FindEntityByName("Heroine");
            Entity bush = FindEntityByName("Bush");
            Entity ropes = FindEntityByName("Ropes");
            Entity tree = FindEntityByName("Tree");

            if (player != null)
            {
                var anim = player.GetComponent<SpriteAnimatorComponent>();
                anim.Play("Heroine");
            }

            if (bush != null)
            {
                var anim = bush.GetComponent<SpriteAnimatorComponent>();
                anim.Play("Bush");
            }

            if (ropes != null)
            {
                var anim = ropes.GetComponent<SpriteAnimatorComponent>();
                anim.Play("Ropes");
            }

            if (tree != null)
            {
                var anim = tree.GetComponent<SpriteAnimatorComponent>();
                anim.Play("Tree");
            }

        }
    }
}
