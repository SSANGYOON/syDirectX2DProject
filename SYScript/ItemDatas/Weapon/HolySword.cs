using Sandbox;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SYScript.ItemDatas
{
    public class HolySword : PlayerWeaponData
    {
        public HolySword() {
            itemName = "성스러운 검";
            spritePath = "assets\\textures\\HolySword.png";
            Type = weaponType.OneHand;
            Damage = 7f;
        }
    }
}
