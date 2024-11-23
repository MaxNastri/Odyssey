#include "Animator.h"
#include "AssetManager.h"
#include "DebugRenderer.h"
#include "OdysseyTime.h"
#include "PropertiesComponent.h"
#include "Scene.h"
#include "Transform.h"

namespace Odyssey
{
	Animator::Animator(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	Animator::~Animator()
	{

		int debu = 0;
	}

	void Animator::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		GUID rigGUID = m_Rig->GetGUID();
		GUID blueprintGUID = m_Blueprint->GetGUID();

		componentNode.WriteData("Type", Animator::Type);
		componentNode.WriteData("Animation Rig", rigGUID.CRef());
		componentNode.WriteData("Animation Blueprint", blueprintGUID.CRef());
	}

	void Animator::Deserialize(SerializationNode& node)
	{
		GUID rigGUID;
		GUID blueprintGUID;
		node.ReadData("Animation Rig", rigGUID.Ref());
		node.ReadData("Animation Blueprint", blueprintGUID.Ref());

		if (rigGUID)
			SetRig(rigGUID);

		if (blueprintGUID)
			SetBlueprint(blueprintGUID);
	}

	void Animator::OnEditorUpdate()
	{
		if (m_BoneGameObjects.size() == 0)
			CreateBoneGameObjects();

		Update();
	}

	void Animator::Update()
	{
		if (m_Enabled && m_Blueprint)
			ProcessKeys();
		else
			ProcessTransforms();
	}

	void Animator::SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
		ResetToBindpose();
	}

	void Animator::SetFloat(const std::string& propertyName, float value)
	{
		m_Blueprint->SetFloat(propertyName, value);
	}

	void Animator::SetBool(const std::string& propertyName, bool value)
	{
		m_Blueprint->SetBool(propertyName, value);
	}

	void Animator::SetInt(const std::string& propertyName, int32_t value)
	{
		m_Blueprint->SetInt(propertyName, value);
	}

	void Animator::SetTrigger(const std::string& propertyName)
	{
		m_Blueprint->SetTrigger(propertyName);
	}

	GUID Animator::GetRigAsset()
	{
		return m_Rig->GetGUID();
	}

	GUID Animator::GetBlueprintAsset()
	{
		return m_Blueprint->GetGUID();
	}

	void Animator::SetRig(GUID guid)
	{
		m_Rig = AssetManager::LoadAsset<AnimationRig>(guid);

		// Resize our final poses to match the bone count
		m_FinalPoses.clear();
		m_FinalPoses.resize(m_Rig->GetBones().size());
	}

	void Animator::SetBlueprint(GUID guid)
	{
		m_Blueprint = AssetManager::LoadAsset<AnimationBlueprint>(guid);
	}

	void Animator::CreateBoneGameObjects()
	{
		// Destroy any existing bone game objects
		DestroyBoneGameObjects();

		// Load the rig and get the bones
		const std::vector<Bone>& bones = m_Rig->GetBones();

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
		const std::vector<Bone>& bones = m_Rig->GetBones();

		float time = m_Playing ? Time::DeltaTime() : 0.0f;

		m_Blueprint->Update();
		auto boneKeys = m_Blueprint->GetKeyframe();

		glm::mat4 animatorInverse = glm::inverse(m_GameObject.GetComponent<Transform>().GetWorldMatrix());
		for (size_t i = 0; i < bones.size(); i++)
		{
			// Get the key for this bone
			const std::string& boneName = bones[i].Name;
			BlendKey& blendKey = boneKeys[boneName];

			Transform& boneTransform = m_BoneGameObjectsMap[boneName].GetComponent<Transform>();
			boneTransform.SetPosition(blendKey.Position);
			boneTransform.SetRotation(blendKey.Rotation);
			boneTransform.SetScale(blendKey.Scale);

			glm::mat4 key = m_Rig->GetGlobalMatrix() * animatorInverse * boneTransform.GetWorldMatrix();
			m_FinalPoses[i] = key * bones[i].InverseBindpose;

			if (m_DebugEnabled)
				DebugDrawKey(key);
		}
	}

	void Animator::ProcessTransforms()
	{
		const std::vector<Bone>& bones = m_Rig->GetBones();

		for (size_t i = 0; i < bones.size(); i++)
		{
			const std::string& boneName = bones[i].Name;

			Transform& boneTransform = m_BoneGameObjectsMap[boneName].GetComponent<Transform>();
			m_FinalPoses[i] = m_Rig->GetRotationOffset() * boneTransform.GetWorldMatrix();
		}
	}

	void Animator::ResetToBindpose()
	{
		const std::vector<Bone>& bones = m_Rig->GetBones();

		for (size_t i = 0; i < bones.size(); i++)
		{
			const std::string& boneName = bones[i].Name;

			Transform& boneTransform = m_BoneGameObjectsMap[boneName].GetComponent<Transform>();
			boneTransform.SetLocalMatrix(mat4(1.0f));

			glm::mat4 key = m_Rig->GetRotationOffset() * boneTransform.GetWorldMatrix();
			m_FinalPoses[i] = key;
		}
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