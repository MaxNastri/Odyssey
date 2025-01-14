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
		RegisterComponentType<ParticleEmitter>(frameworkAssembly, ParticleEmitter::Type);
		RegisterComponentType<ScriptComponent>(frameworkAssembly, ScriptComponent::Type);
		RegisterComponentType<SpriteRenderer>(frameworkAssembly, SpriteRenderer::Type);
		RegisterComponentType<Transform>(frameworkAssembly, Transform::Type);

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

		ADD_INTERNAL_CALL(ParticleEmitter_GetLooping);
		ADD_INTERNAL_CALL(ParticleEmitter_GetEmissionRate);
		ADD_INTERNAL_CALL(ParticleEmitter_GetRadius);
		ADD_INTERNAL_CALL(ParticleEmitter_GetAngle);
		ADD_INTERNAL_CALL(ParticleEmitter_GetDuration);
		ADD_INTERNAL_CALL(ParticleEmitter_GetLifetime);
		ADD_INTERNAL_CALL(ParticleEmitter_GetSize);
		ADD_INTERNAL_CALL(ParticleEmitter_GetSpeed);
		ADD_INTERNAL_CALL(ParticleEmitter_GetStartColor);
		ADD_INTERNAL_CALL(ParticleEmitter_GetEndColor);
		ADD_INTERNAL_CALL(ParticleEmitter_GetShape);

		ADD_INTERNAL_CALL(ParticleEmitter_SetLooping);
		ADD_INTERNAL_CALL(ParticleEmitter_SetEmissionRate);
		ADD_INTERNAL_CALL(ParticleEmitter_SetRadius);
		ADD_INTERNAL_CALL(ParticleEmitter_SetAngle);
		ADD_INTERNAL_CALL(ParticleEmitter_SetDuration);
		ADD_INTERNAL_CALL(ParticleEmitter_SetLifetime);
		ADD_INTERNAL_CALL(ParticleEmitter_SetSize);
		ADD_INTERNAL_CALL(ParticleEmitter_SetSpeed);
		ADD_INTERNAL_CALL(ParticleEmitter_SetStartColor);
		ADD_INTERNAL_CALL(ParticleEmitter_SetEndColor);
		ADD_INTERNAL_CALL(ParticleEmitter_SetShape);

		ADD_INTERNAL_CALL(Prefab_LoadInstance);

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