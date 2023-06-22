using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Kana_BigSword : Weapon
    {

        private float stateTime = 0f;
        public Rigidbody2DComponent body;
        private AudioSource audioSource;

        void OnCreate()
        {
            Active = false;
            body = GetComponent<Rigidbody2DComponent>();
            body.Enable = false;
            m_SpriteRenderer = GetChild("SwordBody").GetComponent<SpriteRendererComponent>();
            audioSource = GetComponent<AudioSource>();
        }

        void OnUpdate(float ts)
        {
            if (!Active && body.Enable) {
                if (stateTime < 1f)
                {
                    Vector4 color = m_SpriteRenderer.Color;
                    color.W = 1 - stateTime;
                    m_SpriteRenderer.Color = color;
                    Vector4 Emission = m_SpriteRenderer.Emission;
                    Emission.W = 1 - stateTime;
                    m_SpriteRenderer.Color = Emission;
                }
                else
                    Pause();
            }
            stateTime += ts;
        }

        public bool Active
        {
            get { return active; }
            set { active = value;
                stateTime = 0f;
            }
            
        }

        void OnActivated()
        {
            Vector4 color = m_SpriteRenderer.Color;
            color.W = 1;
            m_SpriteRenderer.Color = color;
            Vector4 Emission = m_SpriteRenderer.Emission;
            Emission.W = 1;
            m_SpriteRenderer.Color = Emission;
        }

        void OnTriggerEnter(ref Collision2D collsion)
        {
            if (Active)
            {
                if (collsion.CollisionLayer % 2 > 0) {
                    Active = false;
                    audioSource.Play("assets\\soundClip\\Punch (3).wav");
                }

                else
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
            }
        }
    }
}
