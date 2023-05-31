using SY;
using SYScript;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Weapon : Entity
    {
        private TransformAnimatorComponent m_Animator;


        private WeaponData weaponData = null;
        private SpriteRendererComponent m_SpriteRenderer;

        public WeaponData Data
        {
            get { return weaponData; }
            set { weaponData = value; 
                if(weaponData != null)
                {
                    if(m_SpriteRenderer == null)
                        m_SpriteRenderer = GetComponent<SpriteRendererComponent>();
                    if (m_SpriteRenderer != null)
                        m_SpriteRenderer.Texture = weaponData.spritePath;
                    else
                        m_SpriteRenderer.Texture = null;
                }
            }
        }

        public void OnCreate()
        {
            m_Animator = GetComponent<TransformAnimatorComponent>();
        }

        public void Attack(float at = 0.0f)
        { 
            if(m_Animator != null && weaponData != null)
                m_Animator.Play(Enum.GetName(typeof(WeaponData.weaponType), weaponData.Type), at);
        }
    }
}
