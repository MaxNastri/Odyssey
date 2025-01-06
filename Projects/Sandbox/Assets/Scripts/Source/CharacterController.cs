using Odyssey;
using System;

namespace Sandbox
{
    public class CharacterController : Entity
    {
        public float Speed = 10.0f;
        public Prefab DoorPrefab;

        private Transform m_Transform;
        private Animator m_Animator;

        private Entity m_SpawnedDoor;

        private bool allowInstantiate = true;
        protected override void Awake()
        {
            m_Transform = GetComponent<Transform>();
            m_Animator = GetComponent<Animator>();
        }

        protected override void Update()
        {
            HandleMovement();
            if (allowInstantiate && Input.GetKeyPress(KeyCode.Space))
            {
                m_SpawnedDoor = Prefab.LoadInstance(DoorPrefab);
                allowInstantiate = false;
            }
        }

        private void HandleMovement()
        {
            Vector3 inputDirection = GetInputDirection();

            if (inputDirection == Vector3.Zero)
            {
                if (m_Animator != null)
                    m_Animator.SetFloat("Speed", 0.0f);
            }
            else
            {
                if (m_Transform != null)
                {
                    Vector3 forward = m_Transform.Forward * inputDirection.Z * Speed;
                    Vector3 right = m_Transform.Right * inputDirection.X * Speed;
                    Vector3 up = new Vector3(0, inputDirection.Y * Speed, 0);
                    Vector3 velocity = forward + right + up;

                    m_Transform.Position += velocity * Time.DeltaTime;
                }

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
