using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class KanaTail : Weapon
    {
        private Entity Kana;

        void OnCreate()
        {
            Active = false;
            Kana = GetParent();
            m_BoxCollider = GetComponent<BoxCollider2DComponent>();
        }

        void OnActivated()
        {
            Active = true;
        }

        void OnPaused()
        {
            Active = false;
        }

        public bool Active
        {
            get { return active; }
            set { active = value; }
        }

        public bool IgnoreGuard = false;

        void OnTriggerEnter(ref Collision2D collsion)
        {
            if (Active)
            {
                Entity opp = new Entity(collsion.entityID);
                Character player = opp.As<Character>();
                if (player == null)
                    return;
                if (player.Invisible)
                    return;
                bool guard = player.GuardCheck(this);
                if (guard && !IgnoreGuard)
                {
                    player.OnGuarded(this);
                    OnGuarded(player);
                    Kana.As<Character>().OnBlocked();
                }
                else
                {
                    player.OnAttacked(this);
                    OnAttack(player);
                }
            }
        }
    }
}