using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Sandbox.Player;

namespace Sandbox
{
    public class CastleLever : Interactable
    {
        private float stateTime;
        
        private SpriteRendererComponent sr;
        private SpriteRendererComponent stickSr;
        private RevoluteJointComponent rj;

        private bool IsWinding = false;
        public override void Interact()
        {
            if (!interactable)
                return;
            var em = sr.Emission;
            em.W = 0f;
            sr.Emission = em;
            stickSr.Emission = em;
            interactable = false;
            stateTime = 0f;
            IsWinding = true;
        }

        void OnCreate() {
            sr = GetComponent<SpriteRendererComponent>();
            stickSr = FindEntityByName("StickBody").GetComponent<SpriteRendererComponent>();
            interactable = true;
            Entity g2 = FindEntityByName("Ground2");
            rj = g2.GetComponent<RevoluteJointComponent>();

        }
        void OnUpdate(float ts) {
            if (IsWinding)
            {
                rj.AngleRange = new Vector2(-(2 - stateTime) * (float)Math.PI / 6, -(2 - stateTime) * (float)Math.PI / 6);
                stateTime += ts;

                if (stateTime + ts > 2)
                    IsWinding = false;
            }
        }

        void OnTriggerEnter(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 1)) > 0 && interactable)
            {
                var em = sr.Emission;
                em.W = 1f;
                sr.Emission = em;
                stickSr.Emission = em;
            }
        }

        void OnTriggerExit(ref Collision2D collsion)
        {
            if ((collsion.CollisionLayer & (1 << 1)) > 0)
            {
                var em = sr.Emission;
                em.W = 0f;
                sr.Emission = em;
                stickSr.Emission = em;
            }
        }
    }
}
