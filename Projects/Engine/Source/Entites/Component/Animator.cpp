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

	void Animator::Update()
	{
		if (m_Playing)
			m_CurrentTime += ((double)Time::DeltaTime() * 1000.0);

		auto animClip = AssetManager::LoadAnimationClip(m_AnimationClip);
		std::map<std::string, BoneKeyframe>& boneKeyframes = animClip->GetBoneKeyframes();

		double nextFrameTime = animClip->GetFrameTime(m_NextFrame);
		double frameTime = m_NextFrame == 0 ? animClip->GetDuration() : nextFrameTime;

		if ((m_CurrentTime) >= frameTime)
		{
			size_t maxFrames = animClip->GetFrameCount();
			m_PrevFrame = m_NextFrame;
			m_NextFrame = (m_NextFrame + 1) % maxFrames;

			m_CurrentTime = animClip->GetFrameTime(m_PrevFrame);
		}
	}

	const std::vector<glm::mat4>& Animator::GetFinalPoses()
	{
		Update();

		auto animRig = AssetManager::LoadAnimationRig(m_AnimationRig);
		const std::vector<Bone>& bones = animRig->GetBones();

		m_FinalPoses.clear();
		m_FinalPoses.resize(bones.size());

		if (m_AnimationClip)
			ProcessKeys(bones);
		else
			ProcessTransforms(bones);

		return m_FinalPoses;
	}

	void Animator::ProcessKeys(const std::vector<Bone>& bones)
	{
		// Load the clip
		auto animClip = AssetManager::LoadAnimationClip(m_AnimationClip);

		// Get the bone keyframes
		std::map<std::string, BoneKeyframe>& boneKeyframes = animClip->GetBoneKeyframes();

		// Create storage for our bone keys
		std::map<std::string, glm::mat4> boneKeys;

		for (auto& [boneName, boneKeyframe] : boneKeyframes)
		{
			const double prevTime = boneKeyframe.GetFrameTime(m_PrevFrame);
			const double nextTime = boneKeyframe.GetFrameTime(m_NextFrame);
			double totalTime = nextTime == 0.0 ? animClip->GetDuration() : nextTime;
			float blendFactor = (float)(m_CurrentTime - prevTime) / (totalTime - prevTime);
			// TODO: Disable blending for now
			boneKeys[boneName] = boneKeyframe.BlendKeys(m_PrevFrame, m_NextFrame, 0.0f);
		}

		for (size_t i = 0; i < bones.size(); i++)
		{
			// Get the key for this bone
			const std::string& boneName = bones[i].Name;
			glm::mat4 globalKey = boneKeys[boneName];

			m_FinalPoses[i] = globalKey * bones[i].InverseBindpose;
		}
	}

	void Animator::ProcessTransforms(const std::vector<Bone>& bones)
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