using Odyssey;
using System;

namespace Sandbox
{
    public class CharacterController : Entity
    {
        public struct PlayerInput
        {
            public Vector3 Movement;
            public Vector2 Camera;

            public void Clear()
            {
                Movement = new Vector3(0, 0, 0);
                Camera = new Vector2(0, 0);
            }
        }

        public Prefab Fireball;
        public Transform CameraTransform;
        public float CameraSpeed = 1.0f;
        public float MovementSpeed = 10.0f;

        private Transform m_Transform;
        private Animator m_Animator;
        private PlayerInput m_Input;
        private Entity m_SpawnedFireball;

        protected override void Awake()
        {
            m_Transform = GetComponent<Transform>();
            m_Animator = GetComponent<Animator>();
            m_SpawnedFireball = Prefab.LoadInstance(Fireball);
        }

        protected override void Update()
        {
            HandleMovement();
            HandleCamera();
        }

        private void HandleMovement()
        {
            GetInput();

            if (m_Input.Movement == Vector3.Zero)
            {
                if (m_Animator != null)
                    m_Animator.SetFloat("Speed", 0.0f);
            }
            else
            {
                if (m_Transform != null)
                {
                    Vector3 forward = m_Transform.Forward * m_Input.Movement.Z * MovementSpeed;
                    Vector3 right = m_Transform.Right * m_Input.Movement.X * MovementSpeed;
                    Vector3 up = new Vector3(0, m_Input.Movement.Y * MovementSpeed, 0);
                    Vector3 velocity = forward + right + up;

                    m_Transform.Position += velocity * Time.DeltaTime;
                }

                if (m_Animator != null)
                    m_Animator.SetFloat("Speed", MovementSpeed);
            }
        }

        private void HandleCamera()
        {
            if (m_Input.Camera.X != 0 || m_Input.Camera.Y != 0)
            {
                Vector3 yaw = new Vector3(0, 1, 0) * m_Input.Camera.X * CameraSpeed * Time.DeltaTime;
                m_Transform.EulerAngles += yaw;
            }
        }

        private void GetInput()
        {
            m_Input.Clear();

            // Check keyboard input
            if (Input.GetKeyDown(KeyCode.D))
                m_Input.Movement.X += 1.0f;
            if (Input.GetKeyDown(KeyCode.A))
                m_Input.Movement.X -= 1.0f;
            if (Input.GetKeyDown(KeyCode.Space))
                m_Input.Movement.Y += 1.0f;
            if (Input.GetKeyDown(KeyCode.X))
                m_Input.Movement.Y -= 1.0f;
            if (Input.GetKeyDown(KeyCode.W))
                m_Input.Movement.Z += 1.0f;
            if (Input.GetKeyDown(KeyCode.S))
                m_Input.Movement.Z -= 1.0f;

            // Mouse input for the camera
            m_Input.Camera.X = Input.GetMouseAxisHorizontal();
            m_Input.Camera.Y = Input.GetMouseAxisVertical();
        }
    }
}
