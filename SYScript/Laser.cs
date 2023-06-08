using SY;
using SYScript;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Schema;

namespace Sandbox
{
    public class Laser2D : Entity
    {
        private LineRenderer Line;
        private ParticleSystem endPointPs;
        private Entity endPoint;

        public float Range;
        public ulong mask;
        public float LaserRotation
        {
            get { return Rotation.Z; }
            set { Rotation = new Vector3(0,0,value); }
        }

        public void OnCreate()
        {
            Line = GetComponent<LineRenderer>();
            endPoint = GetChild("LaserEndPoint");
            endPointPs = endPoint.GetComponent<ParticleSystem>();
        }

        public void OnUpdate(float ts)
        {
            Vector2 from = WorldPosition.XY;
            Vector2 diff = new Vector2((float)Math.Cos(LaserRotation), (float)Math.Sin(LaserRotation));
            Vector2 to = WorldPosition.XY + diff * Range;
            Physics.RayCast(ref from, ref to, (ushort)mask, out Collision2D col);

            if (col.entityID == 0)
            {
                endPointPs.State = (uint)ParticleSystem.ParticleState.PAUSE;
                Vector2 size = Line.Size;
                size.X = Range;
                Line.Size = size;
            }
            else
            {
                endPointPs.State = (uint)ParticleSystem.ParticleState.NORMAL;
                float length = (col.ContactPoint - WorldPosition.XY).Length();
                endPoint.Translation = new Vector3(length, 0f, 0f);
                Vector2 size = Line.Size;
                size.X = length;
                Line.Size = size;

                float angle = (float)Math.Atan2(col.normal.Y, col.normal.X);

                endPoint.Rotation = new Vector3(0, 0, -Rotation.Z + angle);
            }

        }
    }
}
