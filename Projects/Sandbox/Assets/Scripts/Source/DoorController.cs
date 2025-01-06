using Odyssey;
using System;

namespace Sandbox
{
    public class DoorController : Entity
    {
        private struct Door
        {
            public Transform Transform;
            public float Direction;
            public float Distance;
            public bool IsOpen;
            public bool Animating;

            public Door()
            {
                IsOpen = false;
                Direction = 1.0f;
                Distance = 0.0f;
                Animating = false;
            }
        }

        public Transform Target;
        public Transform LeftDoor;
        public Transform RightDoor;
        public float OpenRadius = 1.0f;
        public float OpenSpeed = 1.0f;
        public float OpenDistance = 1.0f;

        private Transform m_Transform;

        private Door[] m_Doors = new Door[2];

        protected override void Awake()
        {
            m_Transform = GetComponent<Transform>();

            m_Doors[0] = new Door();
            m_Doors[0].Transform = LeftDoor;

            m_Doors[1] = new Door();
            m_Doors[1].Transform = RightDoor;
            m_Doors[1].Direction = 1.0f;
        }

        protected override void Update()
        {
            Vector3 toTarget = Target.Position - m_Transform.Position;

            float distSq = toTarget.Length();

            if (distSq < (OpenRadius * OpenRadius))
            {
                for (int i = 0; i < m_Doors.Length; i++)
                {
                    if (!m_Doors[i].IsOpen)
                    {
                        Animate(ref m_Doors[i]);
                    }
                }
            }
        }

        private void Animate(ref Door door)
        {
            Vector3 slideDirection = door.Transform.Right * door.Direction;
            Vector3 velocity = slideDirection * OpenSpeed * Time.DeltaTime;

            door.Distance += velocity.Length();
            door.Transform.Position += velocity;

            if (door.Distance >= OpenDistance)
            {
                Console.WriteLine("Door open");
                door.IsOpen = true;
            }
        }
    }
}
