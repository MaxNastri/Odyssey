using Odyssey;
using System;

namespace Sandbox 
{
    public class AnimatorTest : Component
    {
        protected override void Update()
        {
            if (Input.GetKeyDown(KeyCode.Space))
            {
                Console.Write("Space");
            }
        }
    }
}
