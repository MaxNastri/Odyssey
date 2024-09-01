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
		RigImportData& rigData = source->GetImporter().GetRigData();
		m_Transform = rigData.GlobalTransform;

		auto& boneMap = rigData.m_BoneMap;

		// Clear our existing data and resize to match the new bone count
		m_Bones.clear();
		m_Bones.resize(boneMap.size());

		for (auto& [boneName, bone] : boneMap)
		{
			m_Bones[bone.Index] = bone;
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

		root.WriteData("Bone Count", m_Bones.size());
		root.WriteData("Global Transform", m_Transform);

		SerializationNode bonesNode = root.CreateSequenceNode("Bones");
		
		for (auto& bone : m_Bones)
		{
			SerializationNode boneNode = bonesNode.AppendChild();
			boneNode.SetMap();
			boneNode.WriteData("Name", bone.Name);
			boneNode.WriteData("Index", bone.Index);
			boneNode.WriteData("Parent Index", bone.ParentIndex);
			boneNode.WriteData("Bindpose", bone.InverseBindpose);
			boneNode.WriteData("Transform", bone.Transform);
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
			root.ReadData("Bone Count", boneCount);
			root.ReadData("Global Transform", m_Transform);

			// Clear and resize to match the bone count
			m_Bones.clear();
			m_Bones.resize(boneCount);

			SerializationNode bonesNode = root.GetNode("Bones");

			assert(bonesNode.IsSequence());
			assert(bonesNode.HasChildren());

			for (size_t i = 0; i < bonesNode.ChildCount(); ++i)
			{
				SerializationNode boneNode = bonesNode.GetChild(i);
				assert(boneNode.IsMap());

				Bone bone;
				boneNode.ReadData("Name", bone.Name);
				boneNode.ReadData("Index", bone.Index);
				boneNode.ReadData("Parent Index", bone.ParentIndex);
				boneNode.ReadData("Bindpose", bone.InverseBindpose);
				boneNode.ReadData("Transform", bone.Transform);

				m_Bones[bone.Index] = bone;
			}
		}
	}
}