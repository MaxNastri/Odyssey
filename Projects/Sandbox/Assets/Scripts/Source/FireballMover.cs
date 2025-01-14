using Odyssey;
using System;

namespace Sandbox
{
    public class FireballMover : Component
    {
        public float MaxSpeed = 1.0f;
        public uint MaxEmissionRate = 100;

        private ParticleEmitter m_Emitter;
        private Transform m_Transform;
        private Vector3 m_Direction;
        private float m_Speed;
        private uint m_StartingEmission = 0;

        public void SetTransform(Vector3 position, Vector3 direction)
        {
            m_Transform.Position = position;
            m_Direction = direction;
            m_Direction.Normalize();
        }

        protected override void Awake()
        {
            m_Transform = GetComponent<Transform>();
            m_Emitter = GetComponent<ParticleEmitter>();
            m_StartingEmission = m_Emitter.EmissionRate;
        }

        protected override void Update()
        {
            if (m_Transform != null)
            {
                if (m_Speed != MaxSpeed)
                    m_Speed = Math.Min(m_Speed + Time.DeltaTime, MaxSpeed);

                m_Transform.Position += m_Direction * m_Speed * Time.DeltaTime;

                uint emission = (uint)Single.Lerp(m_StartingEmission, m_StartingEmission + MaxEmissionRate, m_Speed / MaxSpeed);
                m_Emitter.EmissionRate = emission;
            }
        }
    }
}
