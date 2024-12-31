#include "ScriptBindings.h"
#include "Animator.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "ScriptComponent.h"
#include "Transform.h"
#include "InternalCalls.h"

namespace Odyssey
{
#define ADD_INTERNAL_CALL(func) frameworkAssembly.AddInternalCall("Odyssey.InternalCalls", #func, (void*)InternalCalls::func)

	void ScriptBindings::Initialize(Coral::ManagedAssembly frameworkAssembly)
	{
		RegisterComponentType<Animator>(frameworkAssembly, Animator::Type);
		RegisterComponentType<Camera>(frameworkAssembly, Camera::Type);
		RegisterComponentType<MeshRenderer>(frameworkAssembly, MeshRenderer::Type);
		RegisterComponentType<Transform>(frameworkAssembly, Transform::Type);
		RegisterComponentType<ScriptComponent>(frameworkAssembly, ScriptComponent::Type);
		RegisterComponentType<SpriteRenderer>(frameworkAssembly, SpriteRenderer::Type);

		ADD_INTERNAL_CALL(Animator_IsEnabled);
		ADD_INTERNAL_CALL(Animator_SetFloat);
		ADD_INTERNAL_CALL(Animator_SetBool);
		ADD_INTERNAL_CALL(Animator_SetInt);
		ADD_INTERNAL_CALL(Animator_SetTrigger);

		ADD_INTERNAL_CALL(GameObject_GetName);
		ADD_INTERNAL_CALL(GameObject_SetName);
		ADD_INTERNAL_CALL(GameObject_AddComponent);
		ADD_INTERNAL_CALL(GameObject_HasComponent);
		ADD_INTERNAL_CALL(GameObject_RemoveComponent);

		ADD_INTERNAL_CALL(Transform_GetPosition);
		ADD_INTERNAL_CALL(Transform_SetPosition);
		ADD_INTERNAL_CALL(Transform_GetEulerAngles);
		ADD_INTERNAL_CALL(Transform_SetEulerAngles);
		ADD_INTERNAL_CALL(Transform_GetScale);
		ADD_INTERNAL_CALL(Transform_SetScale);
		ADD_INTERNAL_CALL(Transform_GetForward);
		ADD_INTERNAL_CALL(Transform_GetRight);

		ADD_INTERNAL_CALL(MeshRenderer_GetMesh);
		ADD_INTERNAL_CALL(MeshRenderer_SetMesh);

		ADD_INTERNAL_CALL(SpriteRenderer_GetFill);
		ADD_INTERNAL_CALL(SpriteRenderer_SetFill);
		ADD_INTERNAL_CALL(SpriteRenderer_GetBaseColor);
		ADD_INTERNAL_CALL(SpriteRenderer_SetBaseColor);
		ADD_INTERNAL_CALL(SpriteRenderer_GetSprite);
		ADD_INTERNAL_CALL(SpriteRenderer_SetSprite);

		ADD_INTERNAL_CALL(Texture2D_GetWidth);
		ADD_INTERNAL_CALL(Texture2D_GetHeight);
		ADD_INTERNAL_CALL(Texture2D_GetMipMapsEnabled);

		ADD_INTERNAL_CALL(Input_GetKeyPress);
		ADD_INTERNAL_CALL(Input_GetKeyDown);
		ADD_INTERNAL_CALL(Input_GetKeyUp);
		ADD_INTERNAL_CALL(Input_GetMouseButtonDown);
		ADD_INTERNAL_CALL(Input_GetMouseAxisHorizontal);
		ADD_INTERNAL_CALL(Input_GetMouseAxisVertical);
		ADD_INTERNAL_CALL(Input_GetMousePosition);

		ADD_INTERNAL_CALL(Time_GetDeltaTime);

		frameworkAssembly.UploadInternalCalls();
	}
}