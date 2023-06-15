using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace SYScript.ItemDatas
{
    public class BestFriend : PlayerWeaponData
    {
        public BestFriend() {
            spritePath = "assets\\textures\\BestFriend.png";
            Type = weaponType.OneHand;
        }
    }
}
