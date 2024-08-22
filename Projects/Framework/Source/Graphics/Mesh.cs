using Odyssey;

namespace Odyssey
{
    [NativeObject]
    public class Mesh : Asset<Mesh>
    {
        internal Mesh() : base() { }
        internal Mesh(ulong guid) : base(guid) { }

        public uint VertexCount
        {
            get { unsafe { return InternalCalls.Mesh_GetVertexCount(); } }
        }

        public uint IndexCount
        {
            get { unsafe { return InternalCalls.Mesh_GetIndexCount(); } }
        }
    }
}
