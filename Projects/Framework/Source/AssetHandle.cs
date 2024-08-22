using System.Runtime.InteropServices;

namespace Odyssey
{
    [StructLayout(LayoutKind.Sequential)]
    public struct AssetHandle
    {
        public static readonly AssetHandle Invalid = new AssetHandle(0);

        internal readonly ulong m_Handle;

        public AssetHandle(ulong handle)
        {
            m_Handle = handle;
        }
    }
}
