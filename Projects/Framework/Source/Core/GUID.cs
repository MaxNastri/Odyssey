namespace Odyssey
{
    public struct GUID
    {
        public static readonly GUID Invalid = new GUID(0);

        internal readonly ulong m_GUID;

        public GUID(ulong guid)
        {
            m_GUID = guid;
        }
    }
}
