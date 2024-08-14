
namespace Odyssey
{
    public class GameObject : Object
    {
        public static GameObject Create()
        {
            uint guid = 0;

            unsafe
            {
                guid = InternalCalls.GameObject_Create();
            }

            return new GameObject(guid);
        }

        private GameObject(uint guid)
        {
            this.GUID = guid;
        }
    }
}
