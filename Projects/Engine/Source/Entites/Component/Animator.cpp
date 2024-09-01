#include "Animator.h"
#include "AnimationRig.h"
#include "AssetManager.h"
#include "AnimationClip.h"
#include "OdysseyTime.h"

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
	}

	const std::vector<glm::mat4>& Animator::GetFinalPoses()
	{
		static double time = 0.0;
		time += (double)Time::DeltaTime();

		auto animRig = AssetManager::LoadAnimationRig(m_AnimationRig);
		const std::vector<Bone>& bones = animRig->GetBones();

		m_FinalPoses.clear();
		m_FinalPoses.resize(bones.size());

		if (m_AnimationClip)
			ProcessKeys(time * 1000.0, bones);
		else
			ProcessTransforms(time * 1000.0, bones);

		return m_FinalPoses;
	}

	void Animator::ProcessKeys(double time, const std::vector<Bone>& bones)
	{
		// Load the clip
		auto animClip = AssetManager::LoadAnimationClip(m_AnimationClip);

		if (time > animClip->GetDuration())
			time = std::fmod(time, animClip->GetDuration());

		// Get the bone keyframes
		std::map<std::string, BoneKeyframe>& boneKeyframes = animClip->GetBoneKeyframes();

		// Create storage for our bone keys
		std::map<std::string, glm::mat4> boneKeys;

		for (auto& [boneName, boneKeyframe] : boneKeyframes)
		{
			boneKeys[boneName] = boneKeyframe.GetKey(time, false);
		}

		for (size_t i = 0; i < bones.size(); i++)
		{
			// Get the key for this bone
			const std::string& boneName = bones[i].Name;
			glm::mat4 globalKey = boneKeys[boneName];

			m_FinalPoses[i] = globalKey * bones[i].InverseBindpose;
		}
	}

	void Animator::ProcessTransforms(double time, const std::vector<Bone>& bones)
	{
		for (size_t i = 0; i < bones.size(); i++)
		{
			//glm::mat4 globalTransform = bones[i].Transform;

			//int32_t parentIndex = bones[i].ParentIndex;
			//while (parentIndex != -1)
			//{
			//	// Get the parent's transform and multiply it into our global transform
			//	glm::mat4 parent = bones[parentIndex].Transform;
			//	globalTransform = parent * globalTransform;

			//	// Move the the next parent
			//	parentIndex = bones[parentIndex].ParentIndex;
			//}

			//m_FinalPoses[i] = rigTransform * globalTransform * bones[i].InverseBindpose;
		}
	}
}