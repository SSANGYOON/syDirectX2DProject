using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SY
{
    public struct Collision2D
    {
        public ulong entityID;
        public Vector2 normal;
        public Vector2 ContactPoint;
        public uint CollisionLayer;
    }
}
