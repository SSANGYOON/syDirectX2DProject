using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class BossSceneCollisionEvent : Entity
    {
        private Entity boss;
        void OnCreate()
        {
            boss = FindEntityByName("DarkLady");
        }
        void OnTriggerEnter(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 1)) > 0 )
            {
                if (boss != null)
                {
                    var bossCon = boss.As<BossController>();
                    bossCon.bossState = BossController.BossState.Chain;
                    Destroy();
                }
            }
        }
    }
}
