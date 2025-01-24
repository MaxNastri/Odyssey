using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Odyssey
{
    public class CharacterController : Component
    {
        public Vector3 LinearVelocity
        {
            get
            {
                unsafe
                {
                    Vector3 value;
                    InternalCalls.CharacterController_GetLinearVelocity(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe { InternalCalls.CharacterController_SetLinearVelocity(Entity.GUID, value); }
            }
        }
    }
}
