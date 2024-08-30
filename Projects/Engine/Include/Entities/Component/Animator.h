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

	private:
		GameObject m_GameObject;
		GUID m_AnimationRig;
		CLASS_DECLARATION(Animator);
	};
}