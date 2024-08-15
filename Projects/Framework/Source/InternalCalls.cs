using Coral.Managed.Interop;

namespace Odyssey
{
    internal static unsafe class InternalCalls
    {
#pragma warning disable CS0649 // Variable is never assigned to

        #region GameObject
        internal static delegate* unmanaged<ulong> GameObject_Create;
        internal static delegate* unmanaged<ulong, NativeString> GameObject_GetName;
        internal static delegate* unmanaged<ulong, NativeString, void> GameObject_SetName;
        internal static delegate* unmanaged<ulong, ReflectionType, void> GameObject_AddComponent;
        internal static delegate* unmanaged<ulong, ReflectionType, bool> GameObject_HasComponent;
        internal static delegate* unmanaged<ulong, ReflectionType, bool> GameObject_RemoveComponent;

        #endregion
#pragma warning restore CS0649
    }
}
