using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SYScript
{
    public class ItemData : ScriptableObject
    {
        public string Description { get; set; }
        public string SpritePath { get; set; }
        public ItemData() { 
            Description = string.Empty;
            SpritePath = string.Empty;
        }
    }
}
