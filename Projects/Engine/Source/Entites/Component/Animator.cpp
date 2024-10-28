#include "Animator.h"
#include "AnimationRig.h"
#include "AssetManager.h"
#include "AnimationClip.h"
#include "OdysseyTime.h"
#include "DebugRenderer.h"
#include "Transform.h"
#include "Scene.h"
#include "PropertiesComponent.h"

namespace Odyssey
{
	Animator::Animator(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	void Animator::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", Animator::Type);
		componentNode.WriteData("Animation Rig", m_AnimationRig.CRef());
		componentNode.WriteData("Animation Clip", m_AnimationClip.CRef());
	}

	void Animator::Deserialize(SerializationNode& node)
	{
		node.ReadData("Animation Rig", m_AnimationRig.Ref());
		node.ReadData("Animation Clip", m_AnimationClip.Ref());

		if (m_AnimationRig)
			SetRig(m_AnimationRig);
	}

	void Animator::OnEditorUpdate()
	{
		if (m_BoneGameObjects.size() == 0)
			CreateBoneGameObjects();

		if (m_AnimationClip)
			ProcessKeys();
		else
			ProcessTransforms();
	}

	void Animator::Update()
	{
		if (m_AnimationClip)
			ProcessKeys();
		else
			ProcessTransforms();
	}

	void Animator::SetRig(GUID animationRigGUID)
	{
		m_AnimationRig = animationRigGUID;

		// Load the rig and resize our final poses to match the bone count
		auto rig = AssetManager::LoadAsset<AnimationRig>(m_AnimationRig);
		m_FinalPoses.clear();
		m_FinalPoses.resize(rig->GetBones().size());
		//CreateBoneGameObjects();
	}

	void Animator::SetClip(GUID animationClipGUID)
	{
		m_AnimationClip = animationClipGUID;
	}

	void Animator::CreateBoneGameObjects()
	{
		// Destroy any existing bone game objects
		DestroyBoneGameObjects();

		// Load the rig and get the bones
		auto rig = AssetManager::LoadAsset<AnimationRig>(m_AnimationRig);
		const std::vector<Bone>& bones = rig->GetBones();

		// Resize our bone game objects to match
		m_BoneGameObjects.resize(bones.size());

		Scene* scene = m_GameObject.GetScene();
		for (size_t i = 0; i < bones.size(); i++)
		{
			// Set the animator as the parent by default
			m_BoneGameObjects[i] = scene->CreateGameObject();
			m_BoneGameObjects[i].AddComponent<Transform>();
			m_BoneGameObjects[i].SetParent(m_GameObject);

			// Update the map
			m_BoneGameObjectsMap[bones[i].Name] = m_BoneGameObjects[i];

			// Make sure we don't serialize these transforms
			PropertiesComponent& properties = m_BoneGameObjects[i].GetComponent<PropertiesComponent>();
			properties.Serialize = false;
			properties.Name = bones[i].Name;
		}

		for (size_t i = 0; i < m_BoneGameObjects.size(); i++)
		{
			const Bone& bone = bones[i];
			if (bone.ParentIndex > -1)
			{
				m_BoneGameObjects[i].SetParent(m_BoneGameObjects[bone.ParentIndex]);
			}
		}
	}

	void Animator::DestroyBoneGameObjects()
	{
		for (auto& gameObject : m_BoneGameObjects)
		{
			gameObject.Destroy();
		}

		m_BoneGameObjects.clear();
	}

	void Animator::ProcessKeys()
	{
		// Create storage for our bone keys
		auto rig = AssetManager::LoadAsset<AnimationRig>(m_AnimationRig);
		auto clip = AssetManager::LoadAsset<AnimationClip>(m_AnimationClip);
		const std::vector<Bone>& bones = rig->GetBones();

		float time = m_Playing ? Time::DeltaTime() : 0.0f;
		auto boneKeys = clip->BlendKeys(time);

		glm::mat4 animatorInverse = glm::inverse(m_GameObject.GetComponent<Transform>().GetWorldMatrix());
		for (size_t i = 0; i < bones.size(); i++)
		{
			// Get the key for this bone
			const std::string& boneName = bones[i].Name;
			BlendKey& blendKey = boneKeys[boneName];

			auto& transform = m_BoneGameObjectsMap[boneName].GetComponent<Transform>();
			transform.SetPosition(blendKey.position);
			transform.SetRotation(blendKey.rotation);
			transform.SetScale(blendKey.scale);
			glm::mat4 key = rig->GetGlobalMatrix() * animatorInverse * transform.GetWorldMatrix();
			m_FinalPoses[i] = key * bones[i].InverseBindpose;

			if (m_DebugEnabled)
				DebugDrawKey(key);
		}
	}

	void Animator::ProcessTransforms()
	{
		// Create storage for our bone keys
		auto rig = AssetManager::LoadAsset<AnimationRig>(m_AnimationRig);
		const std::vector<Bone>& bones = rig->GetBones();

		double time = m_Playing ? (double)Time::DeltaTime() : 0.0;

		for (size_t i = 0; i < bones.size(); i++)
		{
			m_FinalPoses[i] = rig->GetGlobalMatrix();
		}

		if (m_DebugEnabled)
			DebugDrawBones();
	}

	void Animator::DebugDrawBones()
	{
		auto& transform = m_GameObject.GetComponent<Transform>();
		for (auto& boneGameObject : m_BoneGameObjects)
		{
			auto& boneTransform = boneGameObject.GetComponent<Transform>();
			glm::mat4 finalPose = boneTransform.GetWorldMatrix();
			glm::vec3 translation;
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(finalPose, scale, rotation, translation, skew, perspective);

			DebugRenderer::AddSphere(translation, 0.025f, glm::vec4(0, 1, 0, 1));
		}
	}

	void Animator::DebugDrawKey(const glm::mat4& key)
	{
		auto& transform = m_GameObject.GetComponent<Transform>();
		glm::mat4 worldSpace = key;
		glm::vec3 translation;
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(worldSpace, scale, rotation, translation, skew, perspective);

		DebugRenderer::AddSphere(translation, 0.025f, glm::vec4(0, 1, 0, 1));
	}

	void Animator::DebugDrawBone(const Bone& bone)
	{
		auto& transform = m_GameObject.GetComponent<Transform>();

		glm::mat4 boneTransform = transform.GetWorldMatrix() * bone.Bindpose;
		glm::vec3 translation;
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(boneTransform, scale, rotation, translation, skew, perspective);

		DebugRenderer::AddSphere(translation, 0.025f, glm::vec4(0, 1, 0, 1));
	}
}