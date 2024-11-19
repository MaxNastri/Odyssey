#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"
#include "AnimationRig.h"
#include "AnimationClipTimeline.h"

namespace Odyssey
{
	class AnimationRig;
	class AnimationBlueprint;

	class Animator
	{
		CLASS_DECLARATION(Odyssey, Animator)
	public:
		Animator() = default;
		Animator(const GameObject& gameObject);

	public:
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

	public:
		bool IsEnabled() { return m_Enabled; }
		GUID GetRigAsset();
		GUID GetBlueprintAsset();
		void SetRig(GUID animationRigGUID);
		void SetBlueprint(GUID animationClipGUID);
		void SetDebugEnabled(bool enabled) { m_DebugEnabled = enabled; }

	public:
		const std::vector<glm::mat4>& GetFinalPoses() { return m_FinalPoses; }

	private:
		void CreateBoneGameObjects();
		void DestroyBoneGameObjects();
		void ProcessKeys();
		void ProcessTransforms();
		void ResetToBindpose();

	private:
		void DebugDrawBones();
		void DebugDrawKey(const glm::mat4& key);
		void DebugDrawBone(const Bone& bone);

	private:
		bool m_Enabled;
		GameObject m_GameObject;
		std::shared_ptr<AnimationRig> m_Rig;
		std::shared_ptr<AnimationBlueprint> m_Blueprint;

	private:
		std::vector<GameObject> m_BoneGameObjects;
		std::unordered_map<std::string, GameObject> m_BoneGameObjectsMap;
		std::map<std::string, glm::mat4> m_BoneTransforms;
		std::vector<glm::mat4> m_FinalPoses;
		bool m_Playing = false;

	private:
		bool m_DebugEnabled = false;
	};

}