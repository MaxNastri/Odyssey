namespace Odyssey
{
    public class Animator : Component
    {
        public bool IsEnabled()
        {
            unsafe { return InternalCalls.Animator_IsEnabled(Entity.GUID); }
        }

        public void SetFloat(string propertyName, float value)
        {
            unsafe { InternalCalls.Animator_SetFloat(Entity.GUID, propertyName, value); }
        }

        public void SetBool(string propertyName, bool value)
        {
            unsafe { InternalCalls.Animator_SetBool(Entity.GUID, propertyName, value); }
        }

        public void SetInt(string propertyName, int value)
        {
            unsafe { InternalCalls.Animator_SetInt(Entity.GUID, propertyName, value); }
        }

        public void SetTrigger(string propertyName)
        {
            unsafe { InternalCalls.Animator_SetTrigger(Entity.GUID, propertyName); }
        }
    }
}
