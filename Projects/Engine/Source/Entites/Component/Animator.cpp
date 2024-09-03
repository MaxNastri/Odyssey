#include "Animator.h"
#include "AnimationRig.h"
#include "AssetManager.h"
#include "AnimationClip.h"
#include "OdysseyTime.h"
#include "DebugRenderer.h"
#include "Transform.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, Animator);

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
		{
			// Load the rig and resize our final poses to match the bone count
			auto rig = AssetManager::LoadAnimationRig(m_AnimationRig);
			m_FinalPoses.clear();
			m_FinalPoses.resize(rig->GetBones().size());
		}

		// Create a timeline for the clip
		if (m_AnimationClip)
			m_Timeline = AnimationClipTimeline(m_AnimationClip);
	}

	void Animator::SetRig(GUID animationRigGUID)
	{
		m_AnimationRig = animationRigGUID;

		// Load the rig and resize our final poses to match the bone count
		auto rig = AssetManager::LoadAnimationRig(m_AnimationRig);
		m_FinalPoses.clear();
		m_FinalPoses.resize(rig->GetBones().size());
	}

	void Animator::SetClip(GUID animationClipGUID)
	{
		m_AnimationClip = animationClipGUID;

		// Create a timeline for the clip
		if (m_AnimationClip)
			m_Timeline = AnimationClipTimeline(m_AnimationClip);
	}

	const std::vector<glm::mat4>& Animator::GetFinalPoses()
	{
		if (m_AnimationClip)
			ProcessKeys();
		else
			ProcessTransforms();

		return m_FinalPoses;
	}

	void Animator::ProcessKeys()
	{
		// Create storage for our bone keys
		auto rig = AssetManager::LoadAnimationRig(m_AnimationRig);
		const std::vector<Bone>& bones = rig->GetBones();

		double time = m_Playing ? (double)Time::DeltaTime() : 0.0;
		auto boneKeys = m_Timeline.BlendKeys(time);

		for (size_t i = 0; i < bones.size(); i++)
		{
			// Get the key for this bone
			const std::string& boneName = bones[i].Name;
			glm::mat4 globalKey = boneKeys[boneName];

			m_FinalPoses[i] = globalKey * bones[i].InverseBindpose;

			if (m_DebugEnabled)
				DebugDrawKey(globalKey);
		}
	}

	void Animator::ProcessTransforms()
	{
		// Create storage for our bone keys
		auto rig = AssetManager::LoadAnimationRig(m_AnimationRig);
		const std::vector<Bone>& bones = rig->GetBones();

		double time = m_Playing ? (double)Time::DeltaTime() : 0.0;

		for (size_t i = 0; i < bones.size(); i++)
		{
			m_FinalPoses[i] = bones[i].Bindpose * bones[i].InverseBindpose;

			if (m_DebugEnabled)
				DebugDrawBone(bones[i]);
		}
	}

	void Animator::DebugDrawKey(const glm::mat4& key)
	{
		auto& transform = m_GameObject.GetComponent<Transform>();
		glm::mat4 worldSpace = transform.GetWorldMatrix() * key;
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