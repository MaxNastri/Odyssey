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

	Animator::Animator(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{
		Deserialize(node);
	}

	void Animator::OnDestroy()
	{
		SetDebugEnabled(false);
	}

	void Animator::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		GUID rigGUID = m_Rig ? m_Rig->GetGUID() : GUID::Empty();
		GUID blueprintGUID = m_Blueprint ? m_Blueprint->GetGUID() : GUID::Empty();

		componentNode.WriteData("Type", Animator::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Animation Rig", rigGUID.CRef());
		componentNode.WriteData("Animation Blueprint", blueprintGUID.CRef());
		componentNode.WriteData("Rig Root", m_RigRootGUID.CRef());
	}

	void Animator::Deserialize(SerializationNode& node)
	{
		GUID rigGUID;
		GUID blueprintGUID;

		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Animation Rig", rigGUID.Ref());
		node.ReadData("Animation Blueprint", blueprintGUID.Ref());
		node.ReadData("Rig Root", m_RigRootGUID.Ref());

		if (rigGUID)
			SetRig(rigGUID);

		if (blueprintGUID)
			SetBlueprint(blueprintGUID);
	}

	void Animator::OnEditorUpdate()
	{
		if (m_Rig && m_Blueprint)
		{
			// Check if we need to spawn/catalog the bones
			if (m_BoneGameObjects.size() == 0)
			{
				if (m_RigRootGUID)
					CatalogBoneGameObjects();
				else
					CreateBoneGameObjects();
			}

			ProcessKeys();
		}
	}

	void Animator::Update()
	{
		if (m_BoneGameObjects.size() == 0)
			CreateBoneGameObjects();

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
		return m_Rig ? m_Rig->GetGUID() : GUID::Empty();
	}

	GUID Animator::GetBlueprintAsset()
	{
		return m_Blueprint ? m_Blueprint->GetGUID() : GUID::Empty();
	}

	void Animator::SetRig(GUID guid)
	{
		// We are switching to a new rig
		if (m_Rig && m_Rig->GetGUID() != guid)
		{
			m_RigRootGUID = GUID::Empty();
			DestroyBoneGameObjects();
		}

		m_Rig = AssetManager::LoadAsset<AnimationRig>(guid);

		// Resize our final poses to match the bone count
		m_FinalPoses.clear();
		m_FinalPoses.resize(m_Rig->GetBones().size());
	}

	void Animator::SetBlueprint(GUID guid)
	{
		m_Blueprint = AssetManager::LoadInstance<AnimationBlueprint>(guid);
	}

	void Animator::SetDebugEnabled(bool enabled)
	{
		if (enabled != m_DebugEnabled)
		{
			m_DebugEnabled = enabled;

			if (m_DebugEnabled)
				m_DebugID = DebugRenderer::Register([this]() { DebugDraw(); });
			else
				DebugRenderer::Deregister(m_DebugID);
		}
	}

	void Animator::CreateBoneGameObjects()
	{
		// Destroy any existing bone game objects
		DestroyBoneGameObjects();

		Scene* scene = m_GameObject.GetScene();

		// Decompose the global matrix so we can apply it to the "rig root"
		glm::vec3 translation;
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_Rig->GetGlobalMatrix(), scale, rotation, translation, skew, perspective);

		// Load the rig and get the bones
		const std::vector<Bone>& bones = m_Rig->GetBones();

		// Resize our bone game objects to match
		m_BoneGameObjects.resize(bones.size());

		// Reserve slot 0 for the "rig root" and apply the global matrix
		m_RigRoot = scene->CreateGameObject();
		m_RigRoot.SetName("Rig");
		m_RigRoot.SetParent(m_GameObject);
		m_RigRootGUID = m_RigRoot.GetGUID();

		Transform& rigRootTransform = m_RigRoot.AddComponent<Transform>();
		rigRootTransform.SetPosition(translation);
		rigRootTransform.SetRotation(rotation);
		rigRootTransform.SetScale(scale);

		// Now create the actual bones
		for (size_t i = 0; i < bones.size(); i++)
		{
			// Set the rig root as the parent by default
			m_BoneGameObjects[i] = scene->CreateGameObject();
			m_BoneGameObjects[i].AddComponent<Transform>();
			m_BoneGameObjects[i].SetParent(m_RigRoot);
			m_BoneGameObjects[i].SetName(bones[i].Name);

			// Update the map
			m_BoneCatalog[bones[i].Name] = m_BoneGameObjects[i];
		}

		// Now set the proper parent hierarchy
		for (size_t i = 0; i < m_BoneGameObjects.size(); i++)
		{
			const Bone& bone = bones[i];
			if (bone.ParentIndex > -1)
				m_BoneGameObjects[i].SetParent(m_BoneGameObjects[bone.ParentIndex]);
		}
	}

	void Animator::DestroyBoneGameObjects()
	{
		// Destroy the rig root
		m_RigRoot.Destroy();
		m_BoneGameObjects.clear();
		m_RigRootGUID = GUID::Empty();
	}

	void Animator::CatalogBoneGameObjects()
	{
		m_RigRoot = m_GameObject.GetScene()->GetGameObject(m_RigRootGUID);

		std::vector<GameObject> children = m_RigRoot.GetAllChildren();
		const std::vector<Bone>& bones = m_Rig->GetBones();

		for (const Bone& bone : bones)
		{
			for (GameObject& child : children)
			{
				if (child.GetName() == bone.Name)
				{
					m_BoneGameObjects.push_back(child);
					m_BoneCatalog[bone.Name] = child;
				}
			}
		}
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

			Transform& boneTransform = m_BoneCatalog[boneName].GetComponent<Transform>();
			boneTransform.SetPosition(blendKey.Position);
			boneTransform.SetRotation(blendKey.Rotation);
			boneTransform.SetScale(blendKey.Scale);

			glm::mat4 key = animatorInverse * boneTransform.GetWorldMatrix();
			m_FinalPoses[i] = key * bones[i].InverseBindpose;
		}
	}

	void Animator::ProcessTransforms()
	{
		const std::vector<Bone>& bones = m_Rig->GetBones();

		for (size_t i = 0; i < bones.size(); i++)
		{
			const std::string& boneName = bones[i].Name;

			Transform& boneTransform = m_BoneCatalog[boneName].GetComponent<Transform>();
			m_FinalPoses[i] = m_Rig->GetRotationOffset() * boneTransform.GetWorldMatrix();
		}
	}

	void Animator::ResetToBindpose()
	{
		const std::vector<Bone>& bones = m_Rig->GetBones();

		for (size_t i = 0; i < bones.size(); i++)
		{
			const std::string& boneName = bones[i].Name;

			Transform& boneTransform = m_BoneCatalog[boneName].GetComponent<Transform>();
			boneTransform.SetLocalMatrix(mat4(1.0f));

			glm::mat4 key = m_Rig->GetRotationOffset() * boneTransform.GetWorldMatrix();
			m_FinalPoses[i] = key;
		}
	}

	void Animator::DebugDraw()
	{
		for (auto& [boneName, boneObject] : m_BoneCatalog)
		{
			Transform& boneTransform = boneObject.GetComponent<Transform>();

			// Decompose the world matrix
			glm::mat4 worldSpace = boneTransform.GetWorldMatrix();
			glm::vec3 translation;
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(worldSpace, scale, rotation, translation, skew, perspective);

			// Add a sphere at the world-space position
			DebugRenderer::AddSphere(translation, 0.025f, glm::vec4(0, 1, 0, 1));
		}
	}
}