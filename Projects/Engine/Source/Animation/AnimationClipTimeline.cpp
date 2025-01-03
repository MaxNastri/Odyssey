#include "AnimationClipTimeline.h"
#include "AssetManager.h"
#include "AnimationClip.h"

namespace Odyssey
{
	AnimationClipTimeline::AnimationClipTimeline(AnimationClip* animationClip)
		: m_AnimationClip(animationClip)
	{
		m_Duration = (float)m_AnimationClip->GetDuration();
		Reset();
	}

	std::map<std::string, BlendKey>& AnimationClipTimeline::BlendKeys(float deltaTime)
	{
		m_CurrentTime += deltaTime;

		// Load the animation clip and get the bone keyframes
		std::map<std::string, BoneKeyframe>& boneKeyframes = m_AnimationClip->GetBoneKeyframes();

		// Set our time back to the previous frame's time to re-sync
		if (m_CurrentTime > m_Duration)
		{
			m_CurrentTime = std::fmod(m_CurrentTime, m_Duration);
			m_PrevFrame = 0;
			m_NextFrame = 1;
		}

		// Get the next frame time
		double nextFrameTime = m_AnimationClip->GetFrameTime(m_NextFrame);
		double frameTime = m_NextFrame == 0 ? m_AnimationClip->GetDuration() : nextFrameTime;

		// Continue advancing frames until we catch up to the correct frame time
		while (m_CurrentTime >= frameTime)
		{
			// TODO: We -1 here because the exporter includes an extra bad frame at the end
			// The real fix is to modify the exporter to include frames up to duration - 1/30.0f.
			size_t maxFrames = m_AnimationClip->GetFrameCount() - 1;

			// Advance 1 frame forward cycling back to 0 when we reach the last frame
			m_PrevFrame = m_NextFrame;
			m_NextFrame = (m_NextFrame + 1) % maxFrames;

			// Get the next frame time
			double nextFrameTime = m_AnimationClip->GetFrameTime(m_NextFrame);
			frameTime = m_NextFrame == 0 ? m_AnimationClip->GetDuration() : nextFrameTime;
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