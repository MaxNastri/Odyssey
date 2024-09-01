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
		GUID GetClip() { return m_AnimationClip; }
		void SetRig(GUID animationRigGUID) { m_AnimationRig = animationRigGUID; }
		void SetClip(GUID animationClipGUID) { m_AnimationClip = animationClipGUID; }

	public:
		const std::vector<glm::mat4>& GetFinalPoses();

	private:
		void ProcessKeys(double time, const std::vector<Bone>& bones);
		void ProcessTransforms(double time, const std::vector<Bone>& bones);

	private:
		GameObject m_GameObject;
		GUID m_AnimationRig;
		GUID m_AnimationClip;
		std::vector<glm::mat4> m_FinalPoses;
		CLASS_DECLARATION(Animator);
	};
}