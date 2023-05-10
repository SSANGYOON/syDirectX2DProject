using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SY;
namespace SYScript
{
    public  class WeaponData : ItemData
    {
        public enum weaponType
        {
            None,
            TwoHand,
            OneHand,
            Dagger,
        }
        
        public weaponType Type{ get; set; }

        public virtual float calculateDamage()
        {
            return 2.0f;
        }
    }
}
