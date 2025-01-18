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
		if (auto source = AssetManager::LoadSourceAsset<SourceModel>(m_SourceAsset))
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
		const AnimationImportData& animationData = source->GetImporter()->GetAnimationData();

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
		root.WriteData("Duration", m_Duration);
		root.WriteData("Bone Keyframe Count", m_BoneKeyframes.size());

		SerializationNode keyframesNode = root.CreateSequenceNode("Bone Keyframes");

		for (auto& [boneName, boneKeyframe] : m_BoneKeyframes)
		{
			SerializationNode boneNode = keyframesNode.AppendChild();
			boneNode.SetMap();
			boneNode.WriteData("Bone Name", boneKeyframe.GetName());

			SerializationNode positionsNode = boneNode.CreateSequenceNode("Position Keys");
			auto positionKeys = boneKeyframe.GetPositionKeys();

			for (auto& positionKey : positionKeys)
			{
				SerializationNode positionNode = positionsNode.AppendChild();
				positionNode.SetMap();
				positionNode.WriteData("Time", positionKey.Time);
				positionNode.WriteData("Value", positionKey.Value);
			}

			SerializationNode rotationsNode = boneNode.CreateSequenceNode("Rotation Keys");
			auto rotationKeys = boneKeyframe.GetRotationKeys();

			for (auto& rotationKey : rotationKeys)
			{
				SerializationNode rotationNode = rotationsNode.AppendChild();
				rotationNode.SetMap();
				rotationNode.WriteData("Time", rotationKey.Time);
				rotationNode.WriteData("Value", rotationKey.Value);
			}

			SerializationNode scalesNode = boneNode.CreateSequenceNode("Scale Keys");
			auto scaleKeys = boneKeyframe.GetScaleKeys();

			for (auto& scaleKey : scaleKeys)
			{
				SerializationNode scaleNode = scalesNode.AppendChild();
				scaleNode.SetMap();
				scaleNode.WriteData("Time", scaleKey.Time);
				scaleNode.WriteData("Value", scaleKey.Value);
			}
		}

		serializer.WriteToDisk(assetPath);
	}

	void AnimationClip::OnSourceModified()
	{
		Load();
	}
}