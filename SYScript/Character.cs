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
        protected float mapHp;

        protected float mp;
        protected float maxMp;

        protected float stateTime = 0f;

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
    }
}
