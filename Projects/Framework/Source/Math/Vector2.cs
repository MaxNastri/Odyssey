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

        public static Vector2 Lerp(Vector2 start, Vector2 end, float ratio)
        {
            float x = start.X + (end.X - start.X) * ratio;
            float y = start.Y + (end.Y - start.Y) * ratio;

            return new Vector2(x, y);
        }

        public override string ToString()
        {
            return $"({X}, {Y})";
        }
    }
}
