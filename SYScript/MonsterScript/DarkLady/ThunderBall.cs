using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class ThunderBall : Weapon
    {
        void OnCreate()
        {
            active = false;
        }

        void OnActivated()
        {
            active = true;
        }

        void OnPaused()
        {
            active = false;
        }

        void OnTriggerEnter(ref Collision2D collsion)
        {
            if (active)
            {
                Entity opp = new Entity(collsion.entityID);
                Character player = opp.As<Character>();
                if (player == null)
                    return;
                if (player.Invisible)
                    return;

                bool guard = player.GuardCheck(this);
                if (guard)
                {
                    player.OnGuarded(this);
                    OnGuarded(player);
                }
                else
                {
                    player.OnAttacked(this);
                    OnAttack(player);
                }

                Pause();
            }
        }
    }
}
