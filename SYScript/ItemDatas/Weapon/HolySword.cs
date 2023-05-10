﻿using Sandbox;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SYScript.ItemDatas
{
    public class HolySword : WeaponData
    {

        public HolySword() {
            spritePath = "assets\\textures\\HolySword.png";
            Type = weaponType.OneHand;
        }
    }
}
