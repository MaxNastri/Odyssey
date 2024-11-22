using Odyssey;
using System;

namespace Sandbox 
{
    public class AnimatorTest : Entity
    {
        protected override void Update()
        {
            if (Input.GetKeyPress(KeyCode.Space))
            {
                Console.Write("Space");
            }
        }
    }
}
