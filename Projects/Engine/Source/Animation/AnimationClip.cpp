#include "AnimationClip.h"
#include "SourceModel.h"
#include "AssetManager.h"

namespace Odyssey
{
	AnimationClip::AnimationClip(const Path& assetPath)
		: Asset(assetPath)
	{
		if (Ref<SourceModel> source = AssetManager::LoadSourceAsset<SourceModel>(m_SourceAsset))
		{
			source->AddOnModifiedListener([this]() { OnSourceModified(); });
			LoadFromSource(source);
		}

		m_Timeline = AnimationClipTimeline(this);
	}

	AnimationClip::AnimationClip(const Path& assetPath, Ref<SourceModel> sourceModel)
		: Asset(assetPath), m_Timeline(this)
	{
		sourceModel->AddOnModifiedListener([this]() { OnSourceModified(); });

		SetSourceAsset(sourceModel->GetGUID());
		LoadFromSource(sourceModel);
	}

	void AnimationClip::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void AnimationClip::Load()
	{
		if (Ref<SourceModel> source = AssetManager::LoadSourceAsset<SourceModel>(m_SourceAsset))
			LoadFromSource(source);
	}

	std::map<std::string, BlendKey>& AnimationClip::BlendKeys(float deltaTime)
	{
		return m_Timeline.BlendKeys(deltaTime);
	}

	void AnimationClip::Reset()
	{
		m_Timeline.Reset();
	}

	void AnimationClip::SetClipIndex(size_t index)
	{
		m_ClipIndex = index;

		Load();
		m_Timeline = AnimationClipTimeline(this);
	}

	float AnimationClip::GetFrameTime(size_t frameIndex)
	{
		for (auto& [boneName, boneKeyframe] : m_BoneKeyframes)
		{
			return boneKeyframe.GetFrameTime(frameIndex);
		}

		return 0.0f;
	}

	size_t AnimationClip::GetFrameCount()
	{
		for (auto& [boneName, boneKeyframe] : m_BoneKeyframes)
		{
			return boneKeyframe.GetPositionKeys().size();
		}
		return 0;
	}

	float AnimationClip::GetProgress()
	{
		return m_Timeline.GetTime() / m_Duration;
	}

	void AnimationClip::LoadFromSource(Ref<SourceModel> source)
	{
		AssetDeserializer deserializer(m_AssetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			root.ReadData("Clip Index", m_ClipIndex);
		}

		const AnimationImportData& animationData = source->GetImporter()->GetAnimationData(m_ClipIndex % source->GetImporter()->GetClipCount());

		m_Name = animationData.Name;
		m_Duration = animationData.Duration;

		m_BoneKeyframes.clear();

		for (auto& [boneName, boneKeyframe] : animationData.BoneKeyframes)
		{
			m_BoneKeyframes[boneName] = BoneKeyframe(boneKeyframe);
		}
	}

	void AnimationClip::SaveToDisk(const Path& assetPath)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the asset metadata first
		SerializeMetadata(serializer);

		root.WriteData("Name", m_Name);
		root.WriteData("Clip Index", m_ClipIndex);
		root.WriteData("Duration", m_Duration);

		serializer.WriteToDisk(assetPath);
	}

	void AnimationClip::OnSourceModified()
	{
		Load();
	}
}