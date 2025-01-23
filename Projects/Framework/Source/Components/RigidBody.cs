using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Odyssey
{
    public class RigidBody : Component
    {
        public Vector3 LinearVelocity
        {
            get
            {
                unsafe
                {
                    Vector3 value;
                    InternalCalls.RigidBody_GetLinearVelocity(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe { InternalCalls.RigidBody_SetLinearVelocity(Entity.GUID, value); }
            }
        }

        public float Friction
        {
            get
            {
                unsafe
                {
                    float value;
                    InternalCalls.RigidBody_GetFriction(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe { InternalCalls.RigidBody_SetFriction(Entity.GUID, value); }
            }
        }

        public float MaxLinearVelocity
        {
            get
            {
                unsafe
                {
                    float value;
                    InternalCalls.RigidBody_GetMaxLinearVelocity(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe { InternalCalls.RigidBody_SetMaxLinearVelocity(Entity.GUID, value); }
            }
        }

        public void AddLinearVelocity(Vector3 velocity)
        {
            unsafe { InternalCalls.RigidBody_AddLinearVelocity(Entity.GUID, velocity); }
        }
    }
}
