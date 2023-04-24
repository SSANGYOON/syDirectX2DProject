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

        public enum WeaponType
        {
            TwoHand,
            OneHand,
            Dagger,
        }

        public WeaponType m_WeaaponType;

        public WeaponType weaponType
        {
            get { return m_WeaaponType; }
            private set { m_WeaaponType = value; }
        }
        public void OnCreate()
        { }

        public void OnUpdate(float timeStep)
        { }
    }
}
