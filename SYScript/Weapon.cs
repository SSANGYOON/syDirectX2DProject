using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Weapon : Entity
    {
        private float _duration = 1.0f;

        public float duration
        {
            get { return _duration; }
            private set { _duration = value; }
        }
        private TransformAnimatorComponent m_Animator;
        public enum WeaponType
        {
            TwoHand,
            OneHand,
            Dagger,
        }

        protected WeaponType m_Type = WeaponType.OneHand;

        public WeaponType weaponType
        {
            get { return m_Type; }
            private set { m_Type = value; }
        }

        public void Attack()
        { 
            if(m_Animator != null)
                m_Animator.Play(Enum.GetName(typeof(WeaponType), m_Type));
        }
    }
}
