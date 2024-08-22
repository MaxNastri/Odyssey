namespace Odyssey
{
    public class Asset<T>
    {
        public AssetHandle Handle { get; internal set; }

        internal Asset()
        {
            Handle = AssetHandle.Invalid;
        }

        internal Asset(ulong guid)
        {
            Handle = new AssetHandle(guid);
        }
    }
}
