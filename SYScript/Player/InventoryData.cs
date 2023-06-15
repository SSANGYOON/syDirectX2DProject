using Sandbox;
using SY;
using SYScript.ItemDatas;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SYScript
{
    public class InventoryData : ScriptableObject
    {
        public enum CATEGORY
        {
            Weapon = 0,
            Spell,
            Consumable,
            Equipment,
            END
        }

        public List<ItemData>[] itemDatas;

        public InventoryData() {

            itemDatas = new List<ItemData>[(int)CATEGORY.END];

            for (int i=0;i< itemDatas.Length;i++)
            {
                itemDatas[i] = new List<ItemData> ();
            }

            itemDatas[(int)CATEGORY.Weapon].Add(new Steeleto());
            itemDatas[(int)CATEGORY.Weapon].Add(new HolySword());
            itemDatas[(int)CATEGORY.Weapon].Add(new BestFriend());
            itemDatas[(int)CATEGORY.Weapon].Add(new ShieldOfLife());
        }
    }
}
