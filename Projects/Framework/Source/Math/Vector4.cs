namespace Odyssey
{
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
