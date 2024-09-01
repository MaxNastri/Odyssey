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
		void Play() { m_Playing = true; }
		void Pause() { m_Playing = !m_Playing; }
		void Update();

	public:
		GUID GetRig() { return m_AnimationRig; }
		GUID GetClip() { return m_AnimationClip; }
		void SetRig(GUID animationRigGUID) { m_AnimationRig = animationRigGUID; }
		void SetClip(GUID animationClipGUID) { m_AnimationClip = animationClipGUID; }

	public:
		const std::vector<glm::mat4>& GetFinalPoses();

	private:
		void ProcessKeys(const std::vector<Bone>& bones);
		void ProcessTransforms(const std::vector<Bone>& bones);

	private:
		GameObject m_GameObject;
		GUID m_AnimationRig;
		GUID m_AnimationClip;

	private:
		std::vector<glm::mat4> m_FinalPoses;
		bool m_Playing = false;
		double m_CurrentTime = 0.0;
		size_t m_PrevFrame = 0;
		size_t m_NextFrame = 0;

		CLASS_DECLARATION(Animator);
	};
}