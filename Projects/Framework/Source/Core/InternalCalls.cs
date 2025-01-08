using Coral.Managed.Interop;

namespace Odyssey
{
    internal static unsafe class InternalCalls
    {
#pragma warning disable CS0649 // Variable is never assigned to

        #region Animator

        internal static delegate* unmanaged<GUID, bool> Animator_IsEnabled;
        internal static delegate* unmanaged<GUID, NativeString, float, void> Animator_SetFloat;
        internal static delegate* unmanaged<GUID, NativeString, bool, void> Animator_SetBool;
        internal static delegate* unmanaged<GUID, NativeString, int, void> Animator_SetInt;
        internal static delegate* unmanaged<GUID, NativeString, void> Animator_SetTrigger;

        #endregion

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
        internal static delegate* unmanaged<GUID, Vector3*, void> Transform_GetForward;
        internal static delegate* unmanaged<GUID, Vector3*, void> Transform_GetRight;

        #endregion

        #region Mesh

        internal static delegate* unmanaged<GUID, uint> Mesh_GetVertexCount;
        internal static delegate* unmanaged<GUID, uint> Mesh_GetIndexCount;

        #endregion

        #region Prefab

        internal static delegate* unmanaged<GUID, GUID*, void> Prefab_LoadInstance;

        #endregion

        #region Texture2D

        internal static delegate* unmanaged<GUID, uint*, void> Texture2D_GetWidth;
        internal static delegate* unmanaged<GUID, uint*, void> Texture2D_GetHeight;
        internal static delegate* unmanaged<GUID, bool*, void> Texture2D_GetMipMapsEnabled;

        #endregion

        #region MeshRenderer

        // <EntityGUID, MeshGUID>
        internal static delegate* unmanaged<GUID, GUID> MeshRenderer_GetMesh;
        // <EntityGUID, MeshGUID>
        internal static delegate* unmanaged<GUID, GUID, void> MeshRenderer_SetMesh;
        #endregion

        #region Sprite Renderer

        internal static delegate* unmanaged<GUID, Vector2*, void> SpriteRenderer_GetFill;
        internal static delegate* unmanaged<GUID, Vector2, void> SpriteRenderer_SetFill;
        internal static delegate* unmanaged<GUID, Color*, void> SpriteRenderer_GetBaseColor;
        internal static delegate* unmanaged<GUID, Color, void> SpriteRenderer_SetBaseColor;
        internal static delegate* unmanaged<GUID, GUID*, void> SpriteRenderer_GetSprite;
        internal static delegate* unmanaged<GUID, GUID, void> SpriteRenderer_SetSprite;

        #endregion

        #region Particle Emitter

        internal static delegate* unmanaged<GUID, bool*, void> ParticleEmitter_GetLooping;
        internal static delegate* unmanaged<GUID, uint*, void> ParticleEmitter_GetEmissionRate;
        internal static delegate* unmanaged<GUID, float*, void> ParticleEmitter_GetRadius;
        internal static delegate* unmanaged<GUID, float*, void> ParticleEmitter_GetAngle;
        internal static delegate* unmanaged<GUID, float*, void> ParticleEmitter_GetDuration;
        internal static delegate* unmanaged<GUID, Vector2*, void> ParticleEmitter_GetLifetime;
        internal static delegate* unmanaged<GUID, Vector2*, void> ParticleEmitter_GetSize;
        internal static delegate* unmanaged<GUID, Vector2*, void> ParticleEmitter_GetSpeed;
        internal static delegate* unmanaged<GUID, Color*, void> ParticleEmitter_GetStartColor;
        internal static delegate* unmanaged<GUID, Color*, void> ParticleEmitter_GetEndColor;
        internal static delegate* unmanaged<GUID, EmitterShape*, void> ParticleEmitter_GetShape;

        internal static delegate* unmanaged<GUID, bool, void> ParticleEmitter_SetLooping;
        internal static delegate* unmanaged<GUID, uint, void> ParticleEmitter_SetEmissionRate;
        internal static delegate* unmanaged<GUID, float, void> ParticleEmitter_SetRadius;
        internal static delegate* unmanaged<GUID, float, void> ParticleEmitter_SetAngle;
        internal static delegate* unmanaged<GUID, float, void> ParticleEmitter_SetDuration;
        internal static delegate* unmanaged<GUID, Vector2, void> ParticleEmitter_SetLifetime;
        internal static delegate* unmanaged<GUID, Vector2, void> ParticleEmitter_SetSize;
        internal static delegate* unmanaged<GUID, Vector2, void> ParticleEmitter_SetSpeed;
        internal static delegate* unmanaged<GUID, Color, void> ParticleEmitter_SetStartColor;
        internal static delegate* unmanaged<GUID, Color, void> ParticleEmitter_SetEndColor;
        internal static delegate* unmanaged<GUID, EmitterShape, void> ParticleEmitter_SetShape;

        #endregion

        #region Input

        internal static delegate* unmanaged<KeyCode, bool> Input_GetKeyPress;
        internal static delegate* unmanaged<KeyCode, bool> Input_GetKeyDown;
        internal static delegate* unmanaged<KeyCode, bool> Input_GetKeyUp;
        internal static delegate* unmanaged<MouseButton, bool> Input_GetMouseButtonDown;
        internal static delegate* unmanaged<float> Input_GetMouseAxisHorizontal;
        internal static delegate* unmanaged<float> Input_GetMouseAxisVertical;
        internal static delegate* unmanaged<Vector2> Input_GetMousePosition;

        #endregion

        #region Time

        internal static delegate* unmanaged<float> Time_GetDeltaTime;

        #endregion

#pragma warning restore CS0649
    }
}
