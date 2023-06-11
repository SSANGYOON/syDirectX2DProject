using SY;
using System;
using System.Collections.Generic;
using System.Linq;


namespace Sandbox
{
    public class BulletGenerator : Entity
    {
        public Entity bulletPref;

        public List<Bullet> bullets;
        public bool reverse = false;
        private float afterShoot = 0;
        private float accTime = 0;
        void OnCreate()
        {
            bullets = new List<Bullet>();
        }

        void OnActivated()
        {
            afterShoot = 0.5f;
        }

        void OnUpdate(float ts)
        {
            float angle = Rotation.Z;

            if (accTime > 3)
                return;
            if (afterShoot >= 0.25f)
            {
                for (int i = 0; i < 3; i++)
                {
                    Bullet bullet = null;
                    int c = bullets.Count;

                    if (c == 0)
                    {
                        bullet = bulletPref.Instantiate().As<Bullet>();
                        bullet._owner = this;
                    }
                    else
                    {
                        bullet = bullets[bullets.Count - 1];
                        bullets.RemoveAt(bullets.Count - 1);
                        bullet.Activate();
                    }

                    var rb = bullet.GetComponent<Rigidbody2DComponent>();
                    bullet.Translation = WorldPosition;

                    rb.LinearVelocity = new Vector2(60 * (float)Math.Cos(angle + i *  Math.PI * 2 / 3), 60 * (float)Math.Sin(angle + i * Math.PI * 2 / 3));
                }
                
                afterShoot -= 0.25f;
            }
            float dir = reverse ? -1 : 1;
            Rotation = new Vector3(0, 0, angle + dir * ts * (float)Math.PI / 6);
            afterShoot += ts;
        }
    }
}
