using SY;
using SYScript;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class SummonedSword : Weapon
    {
        void OnTriggerEnter(ref Collision2D collsion)
        {
            Entity opp = new Entity(collsion.entityID);
            Character enemy = opp.As<Character>();
            if (enemy == null)
                return;
            if (enemy.Invisible)
                return;

            ApplyDamage(enemy);
            enemy.OnAttacked(this);
        }

        public override void ApplyDamage(Character enemy)
        {
            enemy.ReceiveDamage(7);
        }
    }
}
