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
        private BoxCollider2DComponent m_BoxCollider;

        public WeaponData Data
        {
            get { return weaponData; }
            set {
                WeaponData.weaponType prevType = WeaponData.weaponType.None;
                weaponData = value;
                if (weaponData != null)
                {
                    if (weaponData.Type != prevType)
                    {
                        switch (weaponData.Type)
                        {
                            case WeaponData.weaponType.OneHand:
                                m_BoxCollider.Size = new Vector2(3,15);
                                m_BoxCollider.Offset = new Vector2(0, 21);
                                break;

                            case WeaponData.weaponType.Dagger:
                                m_BoxCollider.Size = new Vector2(2, 10);
                                m_BoxCollider.Offset = new Vector2(0, 4);
                                break;
                            default:
                                break;
                        }
                    }
                    m_SpriteRenderer.Texture = weaponData.spritePath;
                }
            }
        }

        public void OnCreate()
        {
            m_Animator = GetComponent<TransformAnimatorComponent>();
            m_SpriteRenderer = GetComponent<SpriteRendererComponent>();
            m_BoxCollider = GetComponent<BoxCollider2DComponent>();
        }

        public void Attack(float at = 0.0f)
        { 
            if(m_Animator != null && weaponData != null)
                m_Animator.Play(Enum.GetName(typeof(WeaponData.weaponType), weaponData.Type), at);
        }
    }
}
