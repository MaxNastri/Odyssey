#pragma once
#include "AnimationBlueprint.h"
#include "AnimationClipTimeline.h"
#include "AnimationRig.h"
#include "AssetSerializer.h"
#include "GameObject.h"

namespace Odyssey
{
	class Animator
	{
		CLASS_DECLARATION(Odyssey, Animator)
	public:
		Animator() = default;
		Animator(const GameObject& gameObject);
		Animator(const GameObject& gameObject, SerializationNode& node);

	public:
		void Awake() { }
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void Play() { m_Playing = true; }
		void Pause() { m_Playing = !m_Playing; }
		void OnEditorUpdate();
		void Update();

	public:
		void SetEnabled(bool enabled);
		void SetFloat(const std::string& propertyName, float value);
		void SetBool(const std::string& propertyName, bool value);
		void SetInt(const std::string& propertyName, int32_t value);
		void SetTrigger(const std::string& propertyName);

	public:
		bool IsEnabled() { return m_Enabled; }
		GUID GetRigAsset();
		GUID GetBlueprintAsset();
		void SetRig(GUID animationRigGUID);
		void SetBlueprint(GUID animationClipGUID);
		void SetDebugEnabled(bool enabled);

	public:
		const std::vector<glm::mat4>& GetFinalPoses() { return m_FinalPoses; }

	private:
		void CreateBoneGameObjects();
		void DestroyBoneGameObjects();
		void CatalogBoneGameObjects();
		void ProcessKeys();
		void ProcessTransforms();
		void ResetToBindpose();

	private:
		void DebugDraw();

	private:
		bool m_Enabled = true;
		GameObject m_GameObject;
		Ref<AnimationRig> m_Rig;
		Ref<AnimationBlueprint> m_Blueprint;

	private:
		GUID m_RigRootGUID;
		GameObject m_RigRoot;
		std::vector<GameObject> m_BoneGameObjects;
		std::unordered_map<std::string, GameObject> m_BoneCatalog;

	private:
		std::map<std::string, glm::mat4> m_BoneTransforms;
		std::vector<glm::mat4> m_FinalPoses;
		bool m_Playing = false;

	private:
		bool m_DebugEnabled = false;
		uint32_t m_DebugID;
	};

}