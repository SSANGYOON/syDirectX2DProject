using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SYScript.ItemDatas
{
    public class Steeleto : PlayerWeaponData
    {
        public Steeleto() {
            itemName = "스틸레토";
            spritePath = "assets\\textures\\Stiletto.png";
            Type = weaponType.Dagger;
            Damage = 5f;
        }
    }
}
