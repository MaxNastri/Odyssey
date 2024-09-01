#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"
#include "AnimationRig.h"

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
		GUID GetRig() { return m_AnimationRig; }
		void SetRig(GUID animationRigGUID);
		const std::vector<glm::mat4>& GetFinalPoses();

	private:
		GameObject m_GameObject;
		GUID m_AnimationRig;
		std::vector<glm::mat4> m_FinalPoses;
		CLASS_DECLARATION(Animator);
	};
}