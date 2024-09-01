#include "AnimationClip.h"
#include "SourceModel.h"
#include "ModelImporter.h"
#include "FBXModelImporter.h"

namespace Odyssey
{
	AnimationClip::AnimationClip(const Path& assetPath)
		: Asset(assetPath)
	{
		LoadFromDisk(assetPath);
	}

	AnimationClip::AnimationClip(const Path& assetPath, std::shared_ptr<SourceModel> sourceModel)
		: Asset(assetPath)
	{
		const FBXModelImporter::AnimationImportData& animationData = sourceModel->GetFBXImporter().GetAnimationData();

		m_Name = animationData.Name;
		m_Duration = animationData.Duration;

		for (auto& [boneName, boneKeyframe] : animationData.BoneKeyframes)
		{
			m_BoneKeyframes[boneName] = BoneKeyframe(boneKeyframe);
		}
	}

	void AnimationClip::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void AnimationClip::Load()
	{
		LoadFromDisk(m_AssetPath);
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

	void AnimationClip::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			root.ReadData("Name", m_Name);
			root.ReadData("Duration", m_Duration);

			size_t boneKeyframeCount = 0;
			root.ReadData("Bone Keyframe Count", boneKeyframeCount);

			SerializationNode keyframesNode = root.GetNode("Bone Keyframes");
			assert(keyframesNode.IsSequence());
			assert(keyframesNode.HasChildren());

			for (size_t i = 0; i < keyframesNode.ChildCount(); i++)
			{
				SerializationNode keyframeNode = keyframesNode.GetChild(i);
				assert(keyframeNode.IsMap());


				// Read and set the name
				std::string boneName;
				keyframeNode.ReadData("Bone Name", boneName);

				auto& boneKeyframe = m_BoneKeyframes[boneName];
				boneKeyframe.SetBoneName(boneName);

				SerializationNode positionsNode = keyframeNode.GetNode("Position Keys");
				assert(positionsNode.IsSequence());
				assert(positionsNode.HasChildren());

				for (size_t posIndex = 0; posIndex < positionsNode.ChildCount(); posIndex++)
				{
					SerializationNode positionNode = positionsNode.GetChild(posIndex);
					assert(positionNode.IsMap());

					// Read and set each position key
					float time;
					glm::vec3 value;
					positionNode.ReadData("Time", time);
					positionNode.ReadData("Value", value);
					boneKeyframe.AddPositionKey(time, value);
				}

				SerializationNode rotationsNode = keyframeNode.GetNode("Rotation Keys");
				assert(rotationsNode.IsSequence());
				assert(rotationsNode.HasChildren());

				for (size_t rotIndex = 0; rotIndex < rotationsNode.ChildCount(); rotIndex++)
				{
					SerializationNode rotationNode = rotationsNode.GetChild(rotIndex);
					assert(rotationNode.IsMap());

					// Read and set each position key
					float time;
					glm::quat value;
					rotationNode.ReadData("Time", time);
					rotationNode.ReadData("Value", value);
					boneKeyframe.AddRotationKey(time, value);
				}

				SerializationNode scalesNode = keyframeNode.GetNode("Scale Keys");
				assert(scalesNode.IsSequence());
				assert(scalesNode.HasChildren());

				for (size_t scaleIndex = 0; scaleIndex < scalesNode.ChildCount(); scaleIndex++)
				{
					SerializationNode scaleNode = scalesNode.GetChild(scaleIndex);
					assert(scaleNode.IsMap());

					// Read and set each position key
					float time;
					glm::vec3 value;
					scaleNode.ReadData("Time", time);
					scaleNode.ReadData("Value", value);
					boneKeyframe.AddScaleKey(time, value);
				}
			}
		}
	}
}