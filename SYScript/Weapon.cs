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
        private float _duration = 1.0f;

        public float duration
        {
            get { return _duration; }
            private set { _duration = value; }
        }
        private TransformAnimatorComponent m_Animator;


        public WeaponData weaponData;

        public void Attack()
        { 
            if(m_Animator != null)
                m_Animator.Play(Enum.GetName(typeof(WeaponData.weaponType), weaponData.Type));
        }
    }
}
