using System.Runtime.InteropServices;

namespace Odyssey
{
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct Vector3
    {
        public float X;
        public float Y;
        public float Z;

        public Vector3()
        {
            X = Y = Z = 0.0f;
        }

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public static Vector3 operator +(Vector3 lhs, Vector3 rhs) => new Vector3(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
        public static Vector3 operator *(Vector3 vector, float scalar) => new Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);

        public float Length() { return X + Y + Z; }
        public void Normalize()
        {
            float length = Length();
            X /= length;
            Y /= length;
            Z /= length;
        }

        public override string ToString()
        {
            return $"({X}, {Y}, {Z})";
        }
    }
}
