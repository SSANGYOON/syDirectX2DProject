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
            spritePath = "assets\\textures\\ShieldOfLifeFront.png";
            spritePath2 = "assets\\textures\\ShieldOfLifeSide.png";
            Type = weaponType.Shield;
        }
    }
}
