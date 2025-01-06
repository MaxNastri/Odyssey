using Odyssey;
using System.Net.NetworkInformation;

namespace Odyssey
{
    [NativeObject]
    public class Prefab : Asset
    {
        public static Entity LoadInstance(Prefab prefab)
        {
            unsafe
            {
                GUID instance = new GUID();
                InternalCalls.Prefab_LoadInstance(prefab.Guid, &instance);
                return new Entity(instance);
            }
        }
    }
}
