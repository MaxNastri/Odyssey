using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Odyssey
{
    public class Transform : Component
    {
        public Vector3 Position
        {
            get
            {
                Vector3 result;
                unsafe { InternalCalls.Transform_GetPosition(Entity.GUID, &result); }
                return result;
            }
        }
    }
}
