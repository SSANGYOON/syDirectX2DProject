using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static SYScript.PlayerWeaponData;
using static System.Net.Mime.MediaTypeNames;

namespace Sandbox.ItemDatas.Skill
{
    public class SummonSword : SkillData
    {
        public SummonSword() {
            itemName = "마법 검 소환";
            spritePath = "assets\\textures\\SummonSword.png";
        }
    }
}
