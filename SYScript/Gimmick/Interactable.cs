using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Interactable : Entity
    {
        protected bool interactable;
        public virtual void Interact() { }
    }
}
