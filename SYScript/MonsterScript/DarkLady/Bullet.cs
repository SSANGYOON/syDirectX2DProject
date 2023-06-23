using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Sandbox.DarkWarlock;

namespace Sandbox
{
    public class Bullet : Weapon
    {
        public BulletGenerator _owner;
        private float lifeTime = 0;

        void OnUpdate(float ts)
        {
            if (lifeTime > 5)
            {
               _owner.bullets.Add(this);
               Pause();
            }
            lifeTime += ts;
        }

        void OnActivated()
        {
            lifeTime = 0;
        }

        void OnTriggerEnter(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 1)) > 0)
            {
                Character player = new Entity(collsion.entityID).As<Character>();
                player.OnAttacked(this);
                _owner.bullets.Add(this);
                Pause();
            }
        }
    }
}
