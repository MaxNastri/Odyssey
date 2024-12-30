namespace Odyssey
{
    public struct Color
    {
        private Vector4 m_Color;

        public float R { get => m_Color.X; set => m_Color.X = value; }
        public float G { get => m_Color.Y; set => m_Color.Y = value; }
        public float B { get => m_Color.Z; set => m_Color.Z = value; }
        public float A { get => m_Color.W; set => m_Color.W = value; }

        public Color(float r, float g, float b, float a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        public static Color White => new Color(1.0f, 1.0f, 1.0f, 1.0f);
        public static Color Black => new Color(0.0f, 0.0f, 0.0f, 1.0f);
        public static Color Red => new Color(1.0f, 0.0f, 0.0f, 1.0f);
        public static Color Green => new Color(0.0f, 1.0f, 0.0f, 1.0f);
        public static Color Blue => new Color(0.0f, 0.0f, 1.0f, 1.0f);
        public static Color Cyan => new Color(0.0f, 1.0f, 1.0f, 1.0f);
    }
}
