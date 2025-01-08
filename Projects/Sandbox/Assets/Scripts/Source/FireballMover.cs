using Odyssey;
using System;

namespace Sandbox
{
    public class FireballMover : Entity
    {
        public float MaxSpeed = 1.0f;
        public uint MaxEmissionRate = 100;

        public ParticleEmitter m_Emitter;
        private Transform m_Transform;
        private float m_Speed;
        private uint m_StartingEmission = 0;

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

                Vector3 forward = m_Transform.Forward;
                m_Transform.Position += forward * m_Speed * Time.DeltaTime;

                uint emission = (uint)Single.Lerp(m_StartingEmission, m_StartingEmission + MaxEmissionRate, m_Speed / MaxSpeed);
                m_Emitter.EmissionRate = emission;
            }
        }
    }
}
