#pragma once
#include "GUID.h"

namespace Odyssey
{
	class AnimationClipTimeline
	{
	public:
		AnimationClipTimeline() = default;
		AnimationClipTimeline(GUID animationClip);

	public:
		const std::unordered_map<std::string, glm::mat4>& BlendKeys(double dt);

	public:
		GUID m_AnimationClip;
		double m_Duration = 0.0;
		double m_CurrentTime = 0.0;
		size_t m_PrevFrame = 0;
		size_t m_NextFrame = 1;
		std::unordered_map<std::string, glm::mat4> m_BoneKeys;
	};
}