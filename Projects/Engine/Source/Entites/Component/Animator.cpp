#include "Animator.h"
#include "AnimationRig.h"

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
}