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
            set
            {
                unsafe { InternalCalls.Transform_SetPosition(Entity.GUID, value); }
            }
        }

        public Vector3 EulerAngles
        {
            get
            {
                Vector3 result;
                unsafe { InternalCalls.Transform_GetEulerAngles(Entity.GUID, &result); }
                return result;
            }
            set
            {
                unsafe { InternalCalls.Transform_SetEulerAngles(Entity.GUID, value); }
            }
        }

        public Vector3 Scale
        {
            get
            {
                Vector3 result;
                unsafe { InternalCalls.Transform_GetScale(Entity.GUID, &result); }
                return result;
            }
            set
            {
                unsafe { InternalCalls.Transform_SetScale(Entity.GUID, value); }
            }
        }
    }
}
