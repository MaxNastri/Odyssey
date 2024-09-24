using Coral.Managed.Interop;

namespace Odyssey
{
    internal static unsafe class InternalCalls
    {
#pragma warning disable CS0649 // Variable is never assigned to

        #region GameObject

        internal static delegate* unmanaged<GUID> GameObject_Create;
        internal static delegate* unmanaged<GUID, NativeString> GameObject_GetName;
        internal static delegate* unmanaged<GUID, NativeString, void> GameObject_SetName;
        internal static delegate* unmanaged<GUID, ReflectionType, void> GameObject_AddComponent;
        internal static delegate* unmanaged<GUID, ReflectionType, bool> GameObject_HasComponent;
        internal static delegate* unmanaged<GUID, ReflectionType, bool> GameObject_RemoveComponent;

        #endregion

        #region Asset

        internal static delegate* unmanaged<GUID, NativeString> Asset_GetName;
        internal static delegate* unmanaged<GUID, NativeString, void> Asset_SetName;

        #endregion

        #region Transform

        internal static delegate* unmanaged<GUID, Vector3*, void> Transform_GetPosition;
        internal static delegate* unmanaged<GUID, Vector3, void> Transform_SetPosition;
        internal static delegate* unmanaged<GUID, Vector3*, void> Transform_GetEulerAngles;
        internal static delegate* unmanaged<GUID, Vector3, void> Transform_SetEulerAngles;
        internal static delegate* unmanaged<GUID, Vector3*, void> Transform_GetScale;
        internal static delegate* unmanaged<GUID, Vector3, void> Transform_SetScale;

        #endregion

        #region Mesh

        internal static delegate* unmanaged<GUID, uint> Mesh_GetVertexCount;
        internal static delegate* unmanaged<GUID, uint> Mesh_GetIndexCount;

        #endregion

        #region MeshRenderer

        // <EntityGUID, MeshGUID>
        internal static delegate* unmanaged<GUID, GUID> MeshRenderer_GetMesh;
        // <EntityGUID, MeshGUID>
        internal static delegate* unmanaged<GUID, GUID, void> MeshRenderer_SetMesh;
        #endregion
        #region Time

        internal static delegate* unmanaged<float> Time_GetDeltaTime;

        #endregion
#pragma warning restore CS0649
    }
}
