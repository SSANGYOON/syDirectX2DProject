using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Skill
    {
        private int m_SkillType = 0;

        public int skillType
        { 
            get { return m_SkillType; }
            private set { m_SkillType = value; }
        }
    }
}
