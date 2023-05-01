using System;

namespace SY
{
    public struct Vector2
    {
        public float X, Y;

        public static Vector2 Zero => new Vector2(0.0f);

        public Vector2(float scalar)
        {
            X = scalar;
            Y = scalar;
        }

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X + b.X, a.Y + b.Y);
        }

        public static Vector2 operator - (Vector2 a, Vector2 b)
        {
            return new Vector2(a.X - b.X, a.Y - b.Y);
        }

        public static Vector2 operator *(Vector2 vector, float scalar)
        {
            return new Vector2(vector.X * scalar, vector.Y * scalar);
        }

        public static Vector2 operator /(Vector2 vector, float scalar)
        {
            return new Vector2(vector.X / scalar, vector.Y / scalar);
        }

        public Vector2 min(Vector2 other)
        {
            return new Vector2(Math.Min(this.X, other.X), Math.Min(this.Y, other.Y));
        }

        public Vector2 max(Vector2 other)
        {
            return new Vector2(Math.Max(this.X, other.X), Math.Max(this.Y, other.Y));
        }

        public Vector2 clamp(Vector2 min, Vector2 max)
        {
            return this.max(min).min(max);
        }

        public float LengthSquared()
        {
            return X * X + Y * Y;
        }

        public float Dot(Vector2 Other)
        { 
            return this.X * Other.X + this.Y * Other.Y;
        }

        public float Length()
        {
            return (float)Math.Sqrt(LengthSquared());
        }


    }
}