using Coral.Managed.Interop;

namespace Odyssey
{
    public class Asset<T>
    {
        public GUID Guid { get; internal set; }

        public string Name
        {
            get { unsafe { return InternalCalls.Asset_GetName(Guid); } }
            set { unsafe { InternalCalls.Asset_SetName(Guid, value); } }
        }

        internal Asset() { Guid = GUID.Invalid; }
        internal Asset(ulong guid) { Guid = new GUID(guid); }
        internal Asset(GUID guid) { Guid = guid; }
    }
}
