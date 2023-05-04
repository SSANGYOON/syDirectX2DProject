using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SYScript
{
    public class InventoryData : ScriptableObject
    {
        public List<WeaponData> weaponDatas = new List<WeaponData>();
        public List<CostumeData> coustumeDatas = new List<CostumeData>();
        public List<AccessoryData> accessoryDatas = new List<AccessoryData>();
        public List<SpellData> spellDatas = new List<SpellData>();
    }
}
