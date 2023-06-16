using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class CastlePortal : Portal
    {

        void OnCreate()
        {
            MainCamera = FindEntityByName("MainCamera");
        }

        internal override void OnTriggerEnter(ref Collision2D collsion)
        {
            base.OnTriggerEnter(ref collsion);
        }
    }
}
