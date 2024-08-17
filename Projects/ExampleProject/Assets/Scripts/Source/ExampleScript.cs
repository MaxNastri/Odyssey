using System;
using Odyssey;

namespace ExampleProject
{
    public class ExampleScript : Entity
    {
        public string MyName = "Max";
        public float speed = 42.0f;
        public int movement = 10;
        public float newSpeedMulti = 0.01f;
        public int anotherMovementThing = -1;

        private Entity myGameObject;
        private Transform myTransform;

        protected override void Awake()
        {
            myGameObject = Entity.Create();
            myGameObject.Name = "ExampleName";

            Transform transform = myGameObject.AddComponent<Transform>();
            Console.WriteLine($"MyGameObject.AddComponent: {transform != null}");

            myTransform = this.GetComponent<Transform>();
            Console.WriteLine($"gameObject.GetComponent: {myTransform != null}");

            bool removed = myGameObject.RemoveComponent<Transform>();
            Console.WriteLine($"MyGameObject.RemoveComponent: {removed}");
        }

        protected override void Update()
        {
        }
    }
}
