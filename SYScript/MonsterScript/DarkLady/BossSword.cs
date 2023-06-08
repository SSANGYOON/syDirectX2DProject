using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class BossSword : Entity
    {
        public SummonedEye _owner = null;
        public Rigidbody2DComponent body = null;
        public void OnCreate()
        {
            body = GetComponent<Rigidbody2DComponent>();
        }

        void OnTriggerEnter(ref Collision2D collsion)
        {
            _owner.OnTriggerEnter(ref collsion);
        }
    }
}
