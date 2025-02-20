using Odyssey;
using System;

namespace Sandbox
{
    public class PlatformMover : Component
    {
        public float Speed = 1.0f;

        private RigidBody m_RigidBody;
        private float m_Time = 0.0f;

        protected override void Awake()
        {
            m_RigidBody = GetComponent<RigidBody>();
        }

        protected override void Update()
        {
            m_Time += Time.DeltaTime;

            float pingPong = MathF.Sin(m_Time);

            m_RigidBody.LinearVelocity = new Vector3(0.0f, pingPong * Speed, 0.0f);
        }
    }
}
