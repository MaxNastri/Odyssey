using System;
using Odyssey;

namespace Sandbox
{
    public class TransformTest : Entity
    {
        public Transform Target;
        public float Speed = 1.0f;
        public Vector3 PosDirection;
        public Vector3 RotDirection;
        public Vector3 ScaleDirection;

        protected override void Awake()
        {
            Console.WriteLine($"[TransformTest.Awake] Print Variables:" +
                $"\nTarget: {Target != null}" +
                $"\nSpeed: {Speed}" +
                $"\nPosDirection: {PosDirection}" +
                $"\nRotDirection: {RotDirection}" +
                $"\nScaleDirection: {ScaleDirection}");
        }

        protected override void Update()
        {
            //if (Target != null)
            //{
            //    Target.Position += (PosDirection * Speed * Time.DeltaTime);
            //    Target.EulerAngles += (RotDirection * Speed * Time.DeltaTime);
            //    Target.Scale += (ScaleDirection * Speed * Time.DeltaTime);
            //}
        }
    }
}
