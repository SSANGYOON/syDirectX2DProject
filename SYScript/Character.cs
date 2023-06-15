using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Character : Entity
    {
        protected Vector4 bloodColor;
        protected Rigidbody2DComponent rigidBody;
        protected BoxCollider2DComponent hitBox;
        protected ParticleSystem blood;

        protected float hp;
        protected float maxHp;

        protected float mp;
        protected float maxMp;

        protected float stateTime = 0f;
        protected bool invisible = false;
        protected bool guard = false;

        public bool Invisible
        { 
            get { return invisible; }
            protected set { invisible = value; }
        }

        public bool Guard
        { 
            get { return guard; }
            private set { guard = value; }
        }

        private bool _flip = false;

        public bool Flip
        {
            get { return _flip; }
            set
            {
                if (_flip != value)
                {
                    if(rigidBody != null)
                        rigidBody.Flip();
                    Vector3 scale = Scale;
                    scale.X *= -1;
                    Scale = scale;
                }
                _flip = value;
            }
        }
        public Character() {
            bloodColor = new Vector4(1, 0, 0, 1);
        }

        public virtual bool GuardCheck(Weapon opp) {
            return false;
        }

        public virtual void OnAttacked(Weapon opp) {
        }

        public virtual void ReceiveDamage(float damage)
        {
            hp -= damage;
        }
        public virtual void OnGuarded(Weapon opp) {
        }

        public virtual void OnBlocked() {
        }
    }
}
