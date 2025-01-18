using Odyssey;
using System;

namespace Sandbox
{
    public class UIManager : Component
    {
        public Texture2D Texture;
        public SpriteRenderer HealthBar;
        public SpriteRenderer ManaBar;
        public float HealthSpeed = 1.0f;
        public float ManaSpeed = 1.5f;

        private float m_HealthDirection = -1.0f;
        private float m_ManaDirection = -1.0f;

        protected override void Update()
        {
            HealthBar.Fill += new Vector2(HealthSpeed * m_HealthDirection * Time.DeltaTime, 0.0f);

            if (HealthBar.Fill.X <= 0.0f || HealthBar.Fill.X >= 1.0f)
                m_HealthDirection *= -1.0f;

            ManaBar.Fill += new Vector2(ManaSpeed * m_ManaDirection * Time.DeltaTime, 0.0f);

            if (ManaBar.Fill.X <= 0.0f || ManaBar.Fill.X >= 1.0f)
                m_ManaDirection *= -1.0f;

            if (Texture != null && Input.GetKeyDown(KeyCode.Alpha0))
                HealthBar.Sprite = Texture;
        }
    }
}
