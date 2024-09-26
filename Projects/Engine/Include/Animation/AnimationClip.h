#pragma once
#include "Asset.h"
#include "BoneKeyframe.hpp"

namespace Odyssey
{
	class SourceModel;

	class AnimationClip : public Asset
	{
		CLASS_DECLARATION(Odyssey, AnimationClip)
	public:
		AnimationClip(const Path& assetPath);
		AnimationClip(const Path& assetPath, std::shared_ptr<SourceModel> sourceModel);

	public:
		void Save();
		void Load();

	public:
		std::map<std::string, BoneKeyframe>& GetBoneKeyframes() { return m_BoneKeyframes; }
		double GetDuration() { return m_Duration; }
		double GetFrameTime(size_t frameIndex);
		size_t GetFrameCount();

	private:
		void SaveToDisk(const Path& assetPath);
		void LoadFromDisk(const Path& assetPath);

	private:
		std::string m_Name;
		double m_Duration;
		std::map<std::string, BoneKeyframe> m_BoneKeyframes;
	};
}