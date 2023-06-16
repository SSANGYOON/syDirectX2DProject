using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class CastleCollisionEvent : Entity
    {
        private Entity kana;
        void OnCreate()
        {
            kana = FindEntityByName("Kana_Phase1");
        }

        void OnTriggerEnter(ref Collision2D collsion)
        {
            kana.As<Kana_Phase1>().FormChange();
            Pause();
        }
    }
}
