using Odyssey;
using System;

namespace ExampleProject
{
    public class AnotherScript : Entity
    {
        public ExampleScript otherScriptRef;
        public Transform otherTransform;
        public float varSpeed = 10.01f;

        protected override void Awake()
        {
            string max = this.otherScriptRef.max;

            Console.WriteLine($"From Another Script: this.otherScriptRef.max = {max}");
        }
    }
}
