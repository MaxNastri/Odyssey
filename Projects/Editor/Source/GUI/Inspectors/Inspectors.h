#pragma once
#include "GUIElement.h"
#include "GameObject.h"
#include "PropertyDrawers.h"
#include "EventSystem.h"

// Components
#include "Components.h"

// Assets
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture2D.h"

// Source assets
#include "SourceModel.h"
#include "SourceShader.h"
#include "SourceTexture.h"

namespace Odyssey
{
	class Inspector : public GUIElement
	{
	public:
		virtual ~Inspector() = default;

	public:
		virtual bool Draw() { return false; }
	};

#pragma region Components

	class AnimatorInspector : public Inspector
	{
	public:
		AnimatorInspector() = default;
		AnimatorInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void OnRigModified(GUID guid);
		void OnBlueprintModified(GUID guid);

	private:
		bool m_AnimatorEnabled;
		GameObject m_GameObject;
		AssetFieldDrawer m_RigDrawer;
		AssetFieldDrawer m_BlueprintDrawer;
	};

	class CameraInspector : public Inspector
	{
	public:
		CameraInspector() = default;
		CameraInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void OnFieldOfViewChanged(float fov);
		void OnNearClipChanged(float nearClip);
		void OnFarClipChanged(float farClip);

	private:
		bool m_CameraEnabled;
		GameObject m_GameObject;
		FloatDrawer m_FieldOfViewDrawer;
		FloatDrawer m_NearClipDrawer;
		FloatDrawer m_FarClipDrawer;
	};

	class LightInspector : public Inspector
	{
	public:
		LightInspector() = default;
		LightInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void OnLightTypeChanged(LightType lightType);
		void OnColorChanged(glm::vec3 color);
		void OnIntensityChanged(float intensity);
		void OnRangeChanged(float range);

	private:
		bool m_LightEnabled;
		GameObject m_GameObject;
		EnumDrawer<LightType> m_LightTypeDrawer;
		ColorPicker m_ColorPicker;
		FloatDrawer m_IntensityDrawer;
		FloatDrawer m_RangeDrawer;
	};

	class MeshRendererInspector : public Inspector
	{
	public:
		MeshRendererInspector() = default;
		MeshRendererInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void InitDrawers();

	private:
		GameObject m_GameObject;
		AssetFieldDrawer m_MeshDrawer;
		std::vector<AssetFieldDrawer> m_MaterialDrawers;
		bool m_MeshRendererEnabled;
	};

	class ParticleEmitterInspector : public Inspector
	{
	public:
		ParticleEmitterInspector() = default;
		ParticleEmitterInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void OnLoopModified(bool loop);
		void OnDurationModified(float duration);
		void OnEmissionRateModified(uint32_t emissionRate);
		void OnMaterialModified(GUID material);
		void OnLifetimeModified(float2 lifetime);
		void OnStartColorModified(float4 color);
		void OnEndColorModified(float4 color);
		void OnSizeModified(float2 size);
		void OnSpeedModified(float2 color);

	private:
		bool m_EmitterEnabled;
		GameObject m_GameObject;
		BoolDrawer m_LoopDrawer;
		FloatDrawer m_DurationDrawer;
		IntDrawer<uint32_t> m_EmissionRateDrawer;
		FloatDrawer m_RadiusDrawer;
		FloatDrawer m_AngleDrawer;
		AssetFieldDrawer m_MaterialDrawer;
		RangeSlider m_LifetimeDrawer;
		RangeSlider m_SizeDrawer;
		RangeSlider m_SpeedDrawer;
		ColorPicker m_StartColorDrawer;
		ColorPicker m_EndColorDrawer;
		EnumDrawer<EmitterShape> m_ShapeDrawer;
	};

	class ScriptInspector : public Inspector
	{
	public:
		ScriptInspector() = default;
		ScriptInspector(GameObject& go);

	public:
		virtual bool Draw() override;
		void UpdateFields();

	private:
		void InitializeDrawers(ScriptComponent* userScript);
		void CreateEntityDrawer(std::string_view fieldName, uint32_t scriptID, uint32_t fieldID, const std::string& typeName, GUID initialValue);
		void CreateAssetDrawer(const std::string& fieldName, const std::string& assetType, uint32_t scriptID, uint32_t fieldID, GUID initialValue);
		void CreateDrawerFromProperty(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage);
		void CreateStringDrawer(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage);
		void OnStringFieldChanged(uint32_t scriptID, uint32_t fieldID, std::string_view newValue);

	private:
		bool m_ScriptEnabled;
		GameObject m_GameObject;
		std::string displayName;
		std::vector<Ref<PropertyDrawer>> drawers;
	};

	class SpriteRendererInspector : public Inspector
	{
	public:
		SpriteRendererInspector() = default;
		SpriteRendererInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void InitDrawers();

	private:
		GameObject m_GameObject;
		AssetFieldDrawer m_SpriteDrawer;
		RangeSlider m_FillDrawer;
		ColorPicker m_BaseColorPicker;
		EnumDrawer<SpriteRenderer::AnchorPosition> m_AnchorDrawer;
		bool m_Enabled;
	};

	class TransformInspector : public Inspector
	{
	public:
		TransformInspector() = default;
		TransformInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void OnPositionChanged(glm::vec3 position);
		void OnRotationChanged(glm::vec3 rotation);
		void OnScaleChanged(glm::vec3 scale);

	private:
		GameObject m_GameObject;
		Vector3Drawer positionDrawer;
		Vector3Drawer rotationDrawer;
		Vector3Drawer scaleDrawer;
	};

	struct SceneModifiedEvent;

	class GameObjectInspector : public Inspector
	{
	public:
		GameObjectInspector() = default;
		GameObjectInspector(GUID guid);
		~GameObjectInspector();

	public:
		virtual bool Draw() override;

	private:
		void CreateInspectors();
		void OnNameChanged(std::string_view name);
		void OnSceneModified(SceneModifiedEvent* eventData);

	private:
		GameObject m_Target;
		GUID m_TargetGUID;
		std::vector<Ref<Inspector>> m_Inspectors;
		std::vector<Inspector> userScriptInspectors;
		StringDrawer m_NameDrawer;
		Ref<IEventListener> m_OnSceneModifiedListener;
	};

	class CharacterControllerInspector : public Inspector
	{
	public:
		CharacterControllerInspector() = default;
		CharacterControllerInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void InitDrawers();

	private:
		GameObject m_GameObject;
		Vector3Drawer m_CenterDrawer;
		FloatDrawer m_RadiusDrawer;
		FloatDrawer m_HeightDrawer;
		BoolDrawer m_InertiaDrawer;
		FloatDrawer m_MaxSlopeDrawer;
		FloatDrawer m_MaxStrengthDrawer;
		FloatDrawer m_PaddingDrawer;
		BoolDrawer m_InnerBodyDrawer;
		bool m_Enabled;
	};

	class BoxColliderInspector : public Inspector
	{
	public:
		BoxColliderInspector() = default;
		BoxColliderInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void InitDrawers();

	private:
		GameObject m_GameObject;
		Vector3Drawer m_CenterDrawer;
		Vector3Drawer m_ExtentsDrawer;
		bool m_Enabled;
	};

	class CapsuleColliderInspector : public Inspector
	{
	public:
		CapsuleColliderInspector() = default;
		CapsuleColliderInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void InitDrawers();

	private:
		GameObject m_GameObject;
		Vector3Drawer m_CenterDrawer;
		FloatDrawer m_RadiusDrawer;
		FloatDrawer m_HeightDrawer;
		bool m_Enabled;
	};

	class SphereColliderInspector : public Inspector
	{
	public:
		SphereColliderInspector() = default;
		SphereColliderInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void InitDrawers();

	private:
		GameObject m_GameObject;
		Vector3Drawer m_CenterDrawer;
		FloatDrawer m_RadiusDrawer;
		bool m_Enabled;
	};

	class RigidBodyInspector : public Inspector
	{
	public:
		RigidBodyInspector() = default;
		RigidBodyInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void InitDrawers();

	private:
		GameObject m_GameObject;
		bool m_Enabled;
		EnumDrawer<PhysicsLayer> m_LayerDrawer;
		Vector3Drawer m_SurfaceVelocityDrawer;
		BoolDrawer m_KinematicDrawer;
		FloatDrawer m_MassDrawer;
		FloatDrawer m_FrictionDrawer;
		FloatDrawer m_MaxLinearVelocityDrawer;
		BoolDrawer m_PushCharacterDrawer;
		BoolDrawer m_ReceiveForceDrawer;
	};

	class FluidBodyInspector : public Inspector
	{
	public:
		FluidBodyInspector() = default;
		FluidBodyInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void InitDrawers();

	private:
		GameObject m_GameObject;
		bool m_Enabled;
		Vector3Drawer m_CenterDrawer;
		Vector3Drawer m_ExtentsDrawer;
		FloatDrawer m_BuoyancyDrawer;
		FloatDrawer m_LinearDragDrawer;
		FloatDrawer m_AngularDragDrawer;
		Vector3Drawer m_FluidVelocityDrawer;
		FloatDrawer m_GravityFactorDrawer;
	};

#pragma endregion

#pragma region Assets

	class MaterialInspector : public Inspector
	{
	public:
		MaterialInspector() = default;
		MaterialInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		Ref<Material> m_Material;
		bool m_Dirty = false;

		StringDrawer m_NameDrawer;
		StringDrawer m_GUIDDrawer;
		AssetFieldDrawer m_ShaderDrawer;
		AssetFieldDrawer m_ColorTextureDrawer;
		AssetFieldDrawer m_NormalTextureDrawer;
		AssetFieldDrawer m_NoiseTextureDrawer;
		ColorPicker m_EmissiveColorDrawer;
		FloatDrawer m_EmissivePowerDrawer;
		FloatDrawer m_AlphaClipDrawer;
		EnumDrawer<RenderQueue> m_RenderQueueDrawer;
		EnumDrawer<BlendMode> m_BlendModeDrawer;
		BoolDrawer m_DepthWriteDrawer;
	};

	class MeshInspector : public Inspector
	{
	public:
		MeshInspector() = default;
		MeshInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		void OnNameChanged(std::string_view name);
		void OnSourceAssetChanged(GUID sourceGUID);

	private:
		Ref<Mesh> m_Mesh;
		StringDrawer m_GUIDDrawer;
		StringDrawer m_NameDrawer;
		StringDrawer m_TypeDrawer;
		StringDrawer m_VertexCountDrawer;
		StringDrawer m_IndexCountDrawer;
		AssetFieldDrawer m_SourceMeshDrawer;
	};

	class ShaderInspector : public Inspector
	{
	public:
		ShaderInspector() = default;
		ShaderInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		void OnNameChanged(std::string_view name);
		void OnSourceAssetChanged(GUID sourceGUID);

	private:
		bool m_Dirty = false;
		Ref<Shader> m_Shader;
		StringDrawer m_GUIDDrawer;
		StringDrawer m_NameDrawer;
		IntDrawer<uint32_t> m_ShaderTypeDrawer;
		AssetFieldDrawer m_SourceShaderDrawer;
	};

	class TextureInspector : public Inspector
	{
	public:
		TextureInspector() = default;
		TextureInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		void OnNameChanged(std::string_view name);
		void OnSourceAssetchanged(GUID sourceGUID);

	private:
		Ref<Texture2D> m_Texture;
		StringDrawer m_GUIDDrawer;
		StringDrawer m_NameDrawer;
		StringDrawer m_TypeDrawer;
		AssetFieldDrawer m_SourceAssetDrawer;
		BoolDrawer m_MipMapDrawer;
		FloatDrawer m_MipBiasDrawer;
		IntDrawer<uint32_t> m_MaxMipCountDrawer;
		uint64_t m_PreviewTexture;
	};

#pragma endregion

#pragma region Source Assets

	class SourceModelInspector : public Inspector
	{
	public:
		SourceModelInspector() = default;
		SourceModelInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		void OnDstPathChanged(std::string_view path) { m_DstPath = path; }

	private:
		Ref<SourceModel> m_Model;
		std::vector<StringDrawer> m_Drawers;
		Path m_DstPath;
	};

	class SourceShaderInspector : public Inspector
	{
	public:
		SourceShaderInspector() = default;
		SourceShaderInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		void OnDstAssetPathChanged(std::string_view assetPath) { m_DstAssetPath = assetPath; }

	private:
		Ref<SourceShader> m_Shader;
		std::string m_DstAssetPath;
		StringDrawer m_ShaderNameDrawer;
		StringDrawer m_ShaderLanguageDrawer;
		StringDrawer m_DstAssetPathDrawer;
		BoolDrawer m_CompiledDrawer;
	};

	class SourceTextureInspector : public Inspector
	{
	public:
		SourceTextureInspector() = default;
		SourceTextureInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		void OnAssetPathChanged(std::string_view assetPath) { m_AssetPath = assetPath; }

	private:
		Ref<SourceTexture> m_Texture;
		StringDrawer m_TextureNameDrawer;
		StringDrawer m_AssetPathDrawer;
		std::string m_AssetPath;
	};

#pragma endregion
}