using System;
using System.IO.IsolatedStorage;

namespace SY
{
    public struct Vector3
    {
        public float X, Y, Z;

        public static Vector3 Zero => new Vector3(0.0f);

        public Vector3(float scalar)
        {
            X = scalar;
            Y = scalar;
            Z = scalar;
        }

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vector3(Vector2 xy, float z)
        {
            X = xy.X;
            Y = xy.Y;
            Z = z;
        }

        public Vector2 XY
        {
            get => new Vector2(X, Y);
            set
            {
                X = value.X;
                Y = value.Y;
            }
        }

        public Vector3 min(Vector3 other)
        {
            return new Vector3(Math.Min(this.X, other.X), Math.Min(this.Y, other.Y), Math.Min(this.Z, other.Z));
        }

        public Vector3 max(Vector3 other)
        {
            return new Vector3(Math.Max(this.X, other.X), Math.Max(this.Y, other.Y), Math.Max(this.Z, other.Z));
        }

        public Vector3 clamp(Vector3 min, Vector3 max)
        {
            return this.max(min).min(max);
        }

        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }

        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        }

        public static Vector3 operator *(Vector3 vector, float scalar)
        {
            return new Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
        }

        public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            return a + (b - a) * t;
        }

        public float Length()
        {
            return (float)Math.Sqrt(X*X + Y*Y + Z*Z);
        }

        public void Normalize()
        {
            float l = this.Length();
            this.X = X/l;
            this.Y = Y/l;
            this.Z = Z/l;
        }
    }
}
