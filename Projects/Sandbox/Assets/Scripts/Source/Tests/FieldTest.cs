using System;
using Odyssey;

namespace Sandbox
{
    public class FieldTest : Entity
    {
        public int Int;
        public uint Uint;
        public float Float;
        public double Double;
        public string String;
        public Entity Entity;
        public Transform Transform;
        public Camera Camera;
        public MeshRenderer Renderer;
        public Vector3 Vector3;

        protected override void Awake()
        {
            Console.WriteLine("[FieldTest] Awake.");
        }
    }
}
