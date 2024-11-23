using System.Runtime.InteropServices;

namespace Odyssey
{
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct Vector2
    {
        public float X;
        public float Y;

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public static Vector2 operator +(Vector2 lhs, Vector2 rhs) => new Vector2(lhs.X + rhs.X, lhs.Y + rhs.Y);
        public static Vector2 operator *(Vector2 vector, float scalar) => new Vector2(vector.X * scalar, vector.Y * scalar);

        public override string ToString()
        {
            return $"({X}, {Y})";
        }
    }
}
