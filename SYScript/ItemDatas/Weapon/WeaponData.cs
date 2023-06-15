using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Metadata.W3cXsd2001;
using System.Text;
using System.Threading.Tasks;
using SY;
namespace SYScript
{
    public class PlayerWeaponData : ItemData
    {
        public enum weaponType
        {
            None,
            TwoHand,
            OneHand,
            Dagger,
            Shield
        }
        
        public weaponType Type{ get; set; }

        protected float _damage;

        public float Damage { get { return _damage; }
                protected set { _damage = value; } }

        public float Duration
        {
            get
            {
                switch(Type)
                {
                    case weaponType.OneHand:
                        return 0.8333f;
                    case weaponType.Dagger:
                        return 0.5f;
                    default:
                        return 1.0f;

                }
            }
        }
    }
}
