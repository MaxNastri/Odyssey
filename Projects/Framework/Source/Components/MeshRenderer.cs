namespace Odyssey
{
    public class MeshRenderer : Component
    {
        public Mesh Mesh
        {
            get { unsafe { return new Mesh(InternalCalls.MeshRenderer_GetMesh(Entity.GUID)); } }
            set
            {
                GUID meshGUID = value != null ? value.Guid : GUID.Invalid;
                unsafe { InternalCalls.MeshRenderer_SetMesh(Entity.GUID, meshGUID); }
            }
        }
    }
}
