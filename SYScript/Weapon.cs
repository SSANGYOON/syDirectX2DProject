using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Weapon : Entity
    {
        protected SpriteRendererComponent m_SpriteRenderer;
        public BoxCollider2DComponent m_BoxCollider;

        protected bool active = false;
        public virtual void OnAttack(Character enemy)
        { }
        public virtual void OnGuarded(Character enemy)
        { }

        public virtual void ApplyDamage(Character enemy)
        { }
    }
}
