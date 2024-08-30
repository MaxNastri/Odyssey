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

	}

	void Animator::Deserialize(SerializationNode& node)
	{

	}

	void Animator::SetRig(std::shared_ptr<AnimationRig> rig)
	{
		m_Rig = rig;
	}
}