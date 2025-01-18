using System.Runtime.InteropServices;

namespace Odyssey
{
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct Vector4
    {
        public float X, Y, Z, W;

        public Vector4()
        {
            X = Y = Y = Z = W = 0;
        }

        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        public static Vector4 operator +(Vector4 lhs, Vector4 rhs)
        {
            return new Vector4(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W);
        }

        public static Vector4 operator *(Vector4 lhs, Vector4 rhs)
        {
            return new Vector4(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z, lhs.W * rhs.W);
        }
        
        public static Vector4 Lerp(Vector4 start, Vector4 end, float ratio)
        {
            float x = start.X + (end.X - start.X) * ratio;
            float y = start.Y + (end.Y - start.Y) * ratio;
            float z = start.Z + (end.Z - start.Z) * ratio;
            float w = start.W + (end.W - start.W) * ratio;

            return new Vector4(x, y, z, w);
        }

        public float Length() {  return X +  Y + Z + W; }

        public void Normalize()
        {
            float length = Length();
            X /= length;
            Y /= length;
            Z /= length;
            W /= length;
        }

        public override string ToString()
        {
            return $"{X}, {Y}, {Z}, {W}";
        }
    }
}
