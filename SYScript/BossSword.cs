using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Sandbox.SummonedEye;

namespace Sandbox
{
    public class BossSword : Entity
    {
        public SummonedEye _owner = null;

        void OnTriggerEnter(ref Collision2D collsion)
        {
            _owner.OnTriggerEnter(ref collsion);
        }
    }
}
