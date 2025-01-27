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

        public void SetFloat(string propertyName, float value, int submesh = 0)
        {
            unsafe { InternalCalls.MeshRenderer_SetFloat(Entity.GUID,  propertyName, value, submesh); }
        }
        public void SetFloat2(string propertyName, Vector2 value, int submesh = 0)
        {
            unsafe { InternalCalls.MeshRenderer_SetFloat2(Entity.GUID, propertyName, value, submesh); }
        }
        public void SetFloat3(string propertyName, Vector3 value, int submesh = 0)
        {
            unsafe { InternalCalls.MeshRenderer_SetFloat3(Entity.GUID, propertyName, value, submesh); }
        }
        public void SetFloat4(string propertyName, Vector4 value, int submesh = 0)
        {
            unsafe { InternalCalls.MeshRenderer_SetFloat4(Entity.GUID, propertyName, value, submesh); }
        }
        public void SetBool(string propertyName, bool value, int submesh = 0)
        {
            unsafe { InternalCalls.MeshRenderer_SetBool(Entity.GUID, propertyName, value, submesh); }
        }
    }
}
