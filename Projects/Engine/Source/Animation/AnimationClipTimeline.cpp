#include "AnimationClipTimeline.h"
#include "AssetManager.h"
#include "AnimationClip.h"

namespace Odyssey
{
	AnimationClipTimeline::AnimationClipTimeline(GUID animationClip)
		: m_AnimationClip(animationClip)
	{
		auto animClip = AssetManager::LoadAsset<AnimationClip>(m_AnimationClip);
		m_Duration = animClip->GetDuration();
	}

	const std::unordered_map<std::string, glm::mat4>& AnimationClipTimeline::BlendKeysOld(double dt)
	{
		m_CurrentTime += dt;

		// Load the animation clip and get the bone keyframes
		auto animClip = AssetManager::LoadAsset<AnimationClip>(m_AnimationClip);
		std::map<std::string, BoneKeyframe>& boneKeyframes = animClip->GetBoneKeyframes();

		// Get the next frame time so we can calculate the proper end time
		double nextFrameTime = animClip->GetFrameTime(m_NextFrame);
		double frameTime = m_NextFrame == 0 ? animClip->GetDuration() : nextFrameTime;

		// Check if we should move to the next frame
		if (m_CurrentTime >= frameTime)
		{
			// Update to the next frame
			size_t maxFrames = animClip->GetFrameCount();
			m_PrevFrame = m_NextFrame;
			m_NextFrame = (m_NextFrame + 1) % maxFrames;

			// Set our time back to the previous frame's time to re-sync
			m_CurrentTime = animClip->GetFrameTime(m_PrevFrame);
		}

		for (auto& [boneName, boneKeyframe] : boneKeyframes)
		{
			const double prevTime = boneKeyframe.GetFrameTime(m_PrevFrame);
			const double nextTime = boneKeyframe.GetFrameTime(m_NextFrame);

			// Calculate the blend factor based on clip times
			double totalTime = nextTime == 0.0 ? animClip->GetDuration() : nextTime;
			float blendFactor = (float)((m_CurrentTime - prevTime) / (totalTime - prevTime));

			// Blend the keys
			m_BoneKeys[boneName] = boneKeyframe.BlendKeysOld(m_PrevFrame, m_NextFrame, blendFactor);
		}

		return m_BoneKeys;
	}

	const std::map<std::string, BlendKey>& AnimationClipTimeline::BlendKeys(double dt)
	{
		m_CurrentTime += dt;

		// Load the animation clip and get the bone keyframes
		auto animClip = AssetManager::LoadAsset<AnimationClip>(m_AnimationClip);
		std::map<std::string, BoneKeyframe>& boneKeyframes = animClip->GetBoneKeyframes();

		// Get the next frame time so we can calculate the proper end time
		double nextFrameTime = animClip->GetFrameTime(m_NextFrame);
		double frameTime = m_NextFrame == 0 ? animClip->GetDuration() : nextFrameTime;

		// Check if we should move to the next frame
		if (m_CurrentTime >= frameTime)
		{
			// Update to the next frame
			size_t maxFrames = animClip->GetFrameCount();
			m_PrevFrame = m_NextFrame;
			m_NextFrame = (m_NextFrame + 1) % maxFrames;

			// Set our time back to the previous frame's time to re-sync
			m_CurrentTime = animClip->GetFrameTime(m_PrevFrame);
		}

		for (auto& [boneName, boneKeyframe] : boneKeyframes)
		{
			const double prevTime = boneKeyframe.GetFrameTime(m_PrevFrame);
			const double nextTime = boneKeyframe.GetFrameTime(m_NextFrame);

			// Calculate the blend factor based on clip times
			double totalTime = nextTime == 0.0 ? animClip->GetDuration() : nextTime;
			float blendFactor = (float)((m_CurrentTime - prevTime) / (totalTime - prevTime));

			// Blend the keys
			m_BlendKeys[boneName] = boneKeyframe.BlendKeys(m_PrevFrame, m_NextFrame, blendFactor);
		}

		return m_BlendKeys;
	}
}