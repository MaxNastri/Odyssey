using Odyssey;
using System;

namespace Sandbox
{
    public class CharacterController : Entity
    {
        public float Speed = 10.0f;

        public SpriteRenderer SpriteRenderer;

        private Transform m_Transform;
        private Animator m_Animator;

        protected override void Awake()
        {
            m_Transform = GetComponent<Transform>();
            m_Animator = GetComponent<Animator>();
        }

        protected override void Update()
        {
            Vector3 inputDirection = GetInputDirection();

            if (Input.GetKeyDown(KeyCode.Alpha1))
                SpriteRenderer.Fill += new Vector2(1.0f * Time.DeltaTime, 0.0f);

            if (Input.GetKeyDown(KeyCode.Alpha2))
                SpriteRenderer.Fill += new Vector2(-1.0f * Time.DeltaTime, 0.0f);

            if (Input.GetKeyDown(KeyCode.Alpha3))
                SpriteRenderer.BaseColor = Color.Cyan;
            else if (Input.GetKeyDown(KeyCode.Alpha4))
                SpriteRenderer.BaseColor = Color.Green;

            if (inputDirection.Length() == 0.0f)
            {
                if (m_Animator != null)
                    m_Animator.SetFloat("Speed", 0.0f);
            }
            else
            {
                if (m_Transform != null)
                    m_Transform.Position += inputDirection * Speed * Time.DeltaTime;

                if (m_Animator != null)
                    m_Animator.SetFloat("Speed", Speed);
            }
        }

        private Vector3 GetInputDirection()
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

            return inputDirection;
        }
    }
}
