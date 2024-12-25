using Odyssey;
using System;

namespace Sandbox
{
    public class CharacterController : Entity
    {
        public float Speed = 10.0f;

        private Transform m_Transform;

        protected override void Awake()
        {
            m_Transform = GetComponent<Transform>();
        }

        protected override void Update()
        {
            Vector3 inputDirection = new Vector3();

            if (Input.GetKeyDown(KeyCode.D))
                inputDirection.X += 1.0f;
            if (Input.GetKeyDown(KeyCode.A))
                inputDirection.X -= 1.0f;
            if (Input.GetKeyDown(KeyCode.Space))
                inputDirection.Y += 1.0f;
            if (Input.GetKeyDown(KeyCode.X))
                inputDirection.Y -= 1.0f;
            if (Input.GetKeyDown(KeyCode.W))
                inputDirection.Z += 1.0f;
            if (Input.GetKeyDown(KeyCode.S))
                inputDirection.Z -= 1.0f;

            if (inputDirection.X != 0.0f || inputDirection.Y != 0.0f || inputDirection.Z != 0.0f)
            {
                if (m_Transform != null)
                    m_Transform.Position += inputDirection * Speed * Time.DeltaTime;
            }
        }

    }
}
