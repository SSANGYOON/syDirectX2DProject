using SYScript;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class ShieldOfLife : PlayerWeaponData
    {
        public ShieldOfLife()
        {
            itemName = "생명의 방패";
            spritePath = "assets\\textures\\ShieldOfLifeFront.png";
            spritePath2 = "assets\\textures\\ShieldOfLifeSide.png";
            Type = weaponType.Shield;
        }
    }
}
