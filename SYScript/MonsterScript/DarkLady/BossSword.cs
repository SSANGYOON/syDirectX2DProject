using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class BossSword : Weapon
    {
        public bool Active
        {
            get { return active; }
            set { active = value; body.Enable = value; }
        }

        private Rigidbody2DComponent body = null;

        public void OnCreate()
        {
            body = GetComponent<Rigidbody2DComponent>();
            m_BoxCollider = GetComponent<BoxCollider2DComponent>();
        }

        void OnPaused()
        {
            Active = false;
        }

        void OnTriggerEnter(ref Collision2D collsion)
        {
            if (active)
            {
                if ((collsion.CollisionLayer & (1 << 1)) > 0)
                {
                    Entity opp = new Entity(collsion.entityID);
                    Character player = opp.As<Character>();
                    if (player == null)
                        return;
                    if (player.Invisible)
                        return;

                    player.OnAttacked(this);
                    OnAttack(player);
                }
                else {
                    FindEntityByName("MainCamera").GetComponent<CameraComponent>().AddOscilation(10.0f);
                    Entity player = FindEntityByName("Player");
                    if (player.As<Player>().Grounded)
                    {
                        var rb = player.GetComponent<Rigidbody2DComponent>();
                        var bc = player.GetComponent<BoxCollider2DComponent>();
                        rb.ApplyLinearImpulse(new Vector2(0, 750 * bc.Size.Y * bc.Size.X), true);
                    }
                    active = false;
                }
            }        
        }
    }
}
