using System;
using Odyssey.Managed.Core;

namespace Example.Managed
{
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class CustomAttribute : Attribute
    {
        public float Value;
    }

    public class ExampleScript : Component
    {
        public string MyName = "Max";
        public float speed = 42.0f;
        public int movement = 10;
        public float newSpeedMulti = 0.01f;
        public int anotherMovementThing = -1;

        public override void Update()
        {
            //Console.WriteLine($"IAmAlive: {this.IAmAlive}, MyName: {this.MyName}, Speed = {this.speed}, Movement: {this.movement}");
        }
    }

    public class AnotherScript : Component
    {
    }
}