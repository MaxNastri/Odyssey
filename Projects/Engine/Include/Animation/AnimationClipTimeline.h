#pragma once
#include "GUID.h"
#include "BoneKeyframe.h"

namespace Odyssey
{
	class AnimationClip;

	class AnimationClipTimeline
	{
	public:
		AnimationClipTimeline() = default;
		~AnimationClipTimeline() = default;
		AnimationClipTimeline(AnimationClip* animationClip);

	public:
		std::map<std::string, BlendKey>& BlendKeys(float deltaTime);
		void Reset();

	public:
		float GetTime() { return m_CurrentTime; }

	private:
		AnimationClip* m_AnimationClip = nullptr;
		float m_Duration = 0.0;
		float m_CurrentTime = 0.0;
		size_t m_PrevFrame = 0;
		size_t m_NextFrame = 1;
		std::unordered_map<std::string, glm::mat4> m_BoneKeys;
		std::map<std::string, BlendKey> m_BlendKeys;
	};
}