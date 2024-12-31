using System;
using System.Diagnostics.CodeAnalysis;
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

        public static Vector3 Zero => new Vector3(0.0f, 0.0f, 0.0f);
        public static Vector3 operator +(Vector3 lhs, Vector3 rhs) => new Vector3(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
        public static Vector3 operator -(Vector3 lhs, Vector3 rhs) => new Vector3(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
        public static Vector3 operator *(Vector3 lhs, Vector3 rhs) => new Vector3(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
        public static Vector3 operator *(Vector3 vector, float scalar) => new Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
        public static bool operator ==(Vector3 lhs, Vector3 rhs) => lhs.Equals(rhs);
        public static bool operator !=(Vector3 lhs, Vector3 rhs) => !lhs.Equals(rhs);

        public float Length() { return MathF.Abs(X) + MathF.Abs(Y) + MathF.Abs(Z); }

        public void Normalize()
        {
            float length = Length();
            X /= length;
            Y /= length;
            Z /= length;
        }

        public override bool Equals([NotNullWhen(true)] object obj)
        {
            if (obj is Vector3 other)
                return X == other.X && Y == other.Y && Z == other.Z;

            return false;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public override string ToString()
        {
            return $"({X}, {Y}, {Z})";
        }
    }
}
