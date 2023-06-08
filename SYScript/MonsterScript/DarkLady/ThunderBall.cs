using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class ThunderBall : Entity
    {
        void OnTriggerEnter(ref Collision2D collsion)
        {
            Pause();
        }
    }
}
