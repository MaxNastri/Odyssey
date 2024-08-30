#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"

namespace Odyssey
{
	class AnimationRig;

	class Animator
	{
	public:
		Animator() = default;
		Animator(const GameObject& gameObject);

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetRig(std::shared_ptr<AnimationRig> rig);

	private:
		GameObject m_GameObject;
		std::shared_ptr<AnimationRig> m_Rig;
		CLASS_DECLARATION(Animator);
	};
}