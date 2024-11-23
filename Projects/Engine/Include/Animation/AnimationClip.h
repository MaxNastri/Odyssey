#pragma once
#include "Asset.h"
#include "AnimationClipTimeline.h"

namespace Odyssey
{
	class SourceModel;

	class AnimationClip : public Asset
	{
		CLASS_DECLARATION(Odyssey, AnimationClip)
	public:
		AnimationClip(const Path& assetPath);
		AnimationClip(const Path& assetPath, Ref<SourceModel> sourceModel);

	public:
		void Save();
		void Load();

	public:
		const std::map<std::string, BlendKey>& BlendKeys(float deltaTime);
		void Reset();

	public:
		std::map<std::string, BoneKeyframe>& GetBoneKeyframes() { return m_BoneKeyframes; }
		float GetDuration() { return m_Duration; }
		float GetFrameTime(size_t frameIndex);
		size_t GetFrameCount();
		float GetProgress();

	private:
		void LoadFromSource(Ref<SourceModel> source);
		void SaveToDisk(const Path& assetPath);

	private:
		void OnSourceModified();

	private:
		std::string m_Name;
		float m_Duration;
		std::map<std::string, BoneKeyframe> m_BoneKeyframes;
		AnimationClipTimeline m_Timeline;
	};
}