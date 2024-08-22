using Odyssey;

namespace Odyssey
{
    [NativeObject]
    public class Mesh : Asset<Mesh>
    {
        internal Mesh() : base() { }
        internal Mesh(ulong guid) : base(guid) { }
    }
}
