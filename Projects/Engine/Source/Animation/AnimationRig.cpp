#include "AnimationRig.h"
#include "SourceModel.h"
#include "ModelImporter.h"

namespace Odyssey
{
	AnimationRig::AnimationRig(const Path& assetPath)
		: Asset(assetPath)
	{
		LoadFromDisk(assetPath);
	}

	AnimationRig::AnimationRig(const Path& assetPath, std::shared_ptr<SourceModel> source)
		: Asset(assetPath)
	{
		auto& boneMap = source->GetImporter().GetRigData().m_BoneMap;
		
		// Clear our existing data and resize to match the new bone count
		m_Bones.clear();
		m_Bindposes.clear();
		m_Bones.resize(boneMap.size());
		m_Bindposes.resize(m_Bones.size());

		for (auto& [boneName, bone] : boneMap)
		{
			m_Bones[bone.Index] = bone;
			m_Bindposes[bone.Index] = bone.InverseBindpose;
		}
	}

	void AnimationRig::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void AnimationRig::Load()
	{
		LoadFromDisk(m_AssetPath);
	}

	void AnimationRig::SaveToDisk(const Path& assetPath)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the asset metadata first
		SerializeMetadata(serializer);

		root.WriteData("BoneCount", m_Bones.size());
		SerializationNode bonesNode = root.CreateSequenceNode("Bones");
		
		for (auto& bone : m_Bones)
		{
			SerializationNode boneNode = bonesNode.AppendChild();
			boneNode.SetMap();
			boneNode.WriteData("Name", bone.Name);
			boneNode.WriteData("Index", bone.Index);
			boneNode.WriteData("Bindpose", bone.InverseBindpose);
		}

		serializer.WriteToDisk(assetPath);
	}

	void AnimationRig::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			// Read in the bone count
			uint32_t boneCount = 0;
			root.ReadData("BoneCount", boneCount);

			// Clear and resize to match the bone count
			m_Bones.clear();
			m_Bindposes.clear();
			m_Bones.resize(boneCount);
			m_Bindposes.reserve(boneCount);

			SerializationNode bonesNode = root.GetNode("Bones");

			assert(bonesNode.IsSequence());
			assert(bonesNode.HasChildren());

			for (size_t i = 0; i < bonesNode.ChildCount(); ++i)
			{
				SerializationNode bonesNode = bonesNode.GetChild(i);
				assert(bonesNode.IsMap());

				Bone bone;
				bonesNode.ReadData("Name", bone.Name);
				bonesNode.ReadData("Index", bone.Index);
				bonesNode.ReadData("Bindpose", bone.InverseBindpose);

				m_Bones[bone.Index] = bone;
				m_Bindposes[bone.Index] = bone.InverseBindpose;
			}
		}
	}
}