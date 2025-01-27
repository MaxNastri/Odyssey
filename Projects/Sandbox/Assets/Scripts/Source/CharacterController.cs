using Odyssey;
using System;

namespace Sandbox
{
    public class CharacterController : Component
    {
        public struct PlayerInput
        {
            public Vector3 Movement;
            public Vector2 Camera;
            public bool Alpha1;

            public void Clear()
            {
                Movement = new Vector3(0, 0, 0);
                Camera = new Vector2(0, 0);
                Alpha1 = false;
            }
        }

        public Prefab Fireball;
        public Transform CameraTransform;
        public Transform FireballTransform;
        public MeshRenderer GrassRenderer;
        public float CameraSpeed = 1.0f;
        public float MovementSpeed = 10.0f;
        public float JumpHeight = 20.0f;

        private Transform m_Transform;
        private Odyssey.CharacterController m_CharacterController;
        private Animator m_Animator;
        private PlayerInput m_Input;
        private Entity m_SpawnedFireball;

        private bool m_IsCasting = false;
        private float m_CastingTimer = 0.0f;
        private const float Casting_Duration = 4.267f;
        private const float Fireball_Spawn_Time = 0.5f;

        protected override void Awake()
        {
            m_Transform = GetComponent<Transform>();
            m_CharacterController = GetComponent<Odyssey.CharacterController>();
            m_Animator = GetComponent<Animator>();
        }

        protected override void Update()
        {
            HandleMovement();
            HandleCamera();

            if (m_IsCasting)
            {
                HandleCasting();
            }
            else if (m_Input.Alpha1)
            {
                m_IsCasting = true;
                m_Animator.SetBool("Casting", true);
            }

            if (GrassRenderer != null)
            {
                GrassRenderer.SetFloat3("PlayerPosition", m_Transform.WorldPosition);
            }
        }

        private void HandleCasting()
        {
            m_CastingTimer = Math.Min(m_CastingTimer + Time.DeltaTime, Casting_Duration);

            if (m_CastingTimer == Casting_Duration)
            {
                m_IsCasting = false;
                m_Animator.SetBool("Casting", false);
                m_CastingTimer = 0.0f;
            }


            // Check if we should spawn the fireball
            if (m_CastingTimer >= (Casting_Duration * Fireball_Spawn_Time) && m_SpawnedFireball == null)
            {
                m_SpawnedFireball = Prefab.LoadInstance(Fireball);
                FireballMover mover = m_SpawnedFireball.GetScript<FireballMover>();
                mover.SetTransform(FireballTransform.WorldPosition, m_Transform.Forward);
                Console.WriteLine("Spawning fireball at: " + FireballTransform.WorldPosition);
            }
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
                    Vector3 velocity = forward + right;

                    if (m_CharacterController.IsGrounded)
                    {
                        Vector3 up = new Vector3(0, m_Input.Movement.Y * JumpHeight, 0);
                        velocity += up;
                    }

                    m_CharacterController.LinearVelocity = velocity;
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

            // Input for skills
            m_Input.Alpha1 = Input.GetKeyDown(KeyCode.Alpha1);
        }
    }
}
