using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace SY
{
    public class ScriptableObject
    {
        public string name;

        public ScriptableObject()
        {
            name = string.Empty;
        }
        public void Serialize(string absolutePath)
        {
            using (StreamWriter wr = new StreamWriter(absolutePath))
            {
                XmlSerializer xs = new XmlSerializer(this.GetType());
                xs.Serialize(wr, this);
            }
        }

        public static ScriptableObject DeSerialize(string absolutePath, Type type)
        {
            ScriptableObject output;
            using (StreamReader fs = new StreamReader(absolutePath))
            {
                XmlSerializer xs = new XmlSerializer(type);
                output = xs.Deserialize(fs) as ScriptableObject;
            }

            return output;
        }
    }
}
