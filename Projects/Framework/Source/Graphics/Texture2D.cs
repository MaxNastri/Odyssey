namespace Odyssey
{
    [NativeObject]
    public class Texture2D : Asset<Texture2D>
    {
        internal Texture2D() : base() { }
        internal Texture2D(ulong guid) : base(guid) { }
        internal Texture2D(GUID guid) : base(guid) { }

        public uint Width
        {
            get
            {
                unsafe
                {
                    uint value = 0;
                    InternalCalls.Texture2D_GetWidth(Guid, &value);
                    return value;
                }
            }
        }

        public uint Height
        {
            get
            {
                unsafe
                {
                    uint value = 0;
                    InternalCalls.Texture2D_GetHeight(Guid, &value);
                    return value;
                }
            }
        }

        public bool MipMapsEnabled
        {
            get
            {
                unsafe
                {
                    bool value = false;
                    InternalCalls.Texture2D_GetMipMapsEnabled(Guid, &value);
                    return value;
                }
            }
        }
    }
}
