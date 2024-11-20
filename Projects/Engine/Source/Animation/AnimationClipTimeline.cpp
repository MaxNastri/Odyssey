#include "AnimationClipTimeline.h"
#include "AssetManager.h"
#include "AnimationClip.h"

namespace Odyssey
{
	AnimationClipTimeline::AnimationClipTimeline(AnimationClip* animationClip)
		: m_AnimationClip(animationClip)
	{
		m_Duration = (float)m_AnimationClip->GetDuration();
	}

	const std::map<std::string, BlendKey>& AnimationClipTimeline::BlendKeys(float deltaTime)
	{
		m_CurrentTime += deltaTime;

		// Load the animation clip and get the bone keyframes
		std::map<std::string, BoneKeyframe>& boneKeyframes = m_AnimationClip->GetBoneKeyframes();

		// Get the next frame time so we can calculate the proper end time
		double nextFrameTime = m_AnimationClip->GetFrameTime(m_NextFrame);
		double frameTime = m_NextFrame == 0 ? m_AnimationClip->GetDuration() : nextFrameTime;

		// Check if we should move to the next frame
		if (m_CurrentTime >= frameTime)
		{
			// Update to the next frame
			size_t maxFrames = m_AnimationClip->GetFrameCount();
			m_PrevFrame = m_NextFrame;
			m_NextFrame = (m_NextFrame + 1) % maxFrames;

			// Set our time back to the previous frame's time to re-sync
			m_CurrentTime = m_AnimationClip->GetFrameTime(m_PrevFrame);
		}

		for (auto& [boneName, boneKeyframe] : boneKeyframes)
		{
			const double prevTime = boneKeyframe.GetFrameTime(m_PrevFrame);
			const double nextTime = boneKeyframe.GetFrameTime(m_NextFrame);

			// Calculate the blend factor based on clip times
			double totalTime = nextTime == 0.0 ? m_AnimationClip->GetDuration() : nextTime;
			float blendFactor = (float)((m_CurrentTime - prevTime) / (totalTime - prevTime));

			// Blend the keys
			m_BlendKeys[boneName] = boneKeyframe.BlendKeys(m_PrevFrame, m_NextFrame, blendFactor);
		}

		return m_BlendKeys;
	}

	void AnimationClipTimeline::Reset()
	{
		m_CurrentTime = 0.0;
		m_PrevFrame = 0;
		m_NextFrame = 1;
	}
}