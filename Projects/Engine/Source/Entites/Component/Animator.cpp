#include "Animator.h"
#include "AnimationRig.h"
#include "AssetManager.h"

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
		componentNode.WriteData("AnimationRig", m_AnimationRig.CRef());
	}

	void Animator::Deserialize(SerializationNode& node)
	{
		node.ReadData("AnimationRig", m_AnimationRig.Ref());
	}

	void Animator::SetRig(GUID animationRigGUID)
	{
		m_AnimationRig = animationRigGUID;
	}

	const std::vector<glm::mat4>& Animator::GetFinalPoses()
	{
		m_FinalPoses.clear();

		auto animRig = AssetManager::LoadAnimationRig(m_AnimationRig);

		std::vector<Bone> bones = animRig->GetBones();

		m_FinalPoses.resize(bones.size());

		for (size_t i = 0; i < bones.size(); i++)
		{
			glm::mat4 globalTransform = bones[i].Transform;

			int32_t parentIndex = bones[i].ParentIndex;
			while (parentIndex != -1)
			{
				// Get the parent's transform and multiply it into our global transform
				glm::mat4 parent = bones[parentIndex].Transform;
				globalTransform = parent * globalTransform;

				// Move the the next parent
				parentIndex = bones[parentIndex].ParentIndex;
			}

			m_FinalPoses[i] = animRig->GetTransform() * globalTransform * bones[i].InverseBindpose;
		}

		return m_FinalPoses;
	}
}