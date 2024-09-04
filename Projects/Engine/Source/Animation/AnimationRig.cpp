#include "AnimationRig.h"
#include "SourceModel.h"
#include "FBXModelImporter.h"

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
		const RigImportData& rigData = source->GetImporter()->GetRigData();
				auto& boneMap = rigData.Bones;

		// Clear our existing data and resize to match the new bone count
		m_Bones.clear();
		m_Bones.resize(boneMap.size());

		m_GlobalMatrix = rigData.GlobalMatrix;

		for (auto& [boneName, bone] : boneMap)
		{
			auto& newBone = m_Bones[bone.Index];
			newBone.Name = bone.Name;
			newBone.Index = bone.Index;
			newBone.ParentIndex = bone.ParentIndex;
			newBone.InverseBindpose = bone.inverseBindpose;
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

		root.WriteData("Root Bone", m_RootBone);
		root.WriteData("Global Matrix", m_GlobalMatrix);
		root.WriteData("Bone Count", m_Bones.size());

		SerializationNode bonesNode = root.CreateSequenceNode("Bones");
		
		for (auto& bone : m_Bones)
		{
			SerializationNode boneNode = bonesNode.AppendChild();
			boneNode.SetMap();
			boneNode.WriteData("Name", bone.Name);
			boneNode.WriteData("Index", bone.Index);
			boneNode.WriteData("Parent Index", bone.ParentIndex);
			boneNode.WriteData("Inverse Bindpose", bone.InverseBindpose);
		}

		serializer.WriteToDisk(assetPath);
	}

	void AnimationRig::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			// Read in the global matrix
			root.ReadData("Root Bone", m_RootBone);
			root.ReadData("Global Matrix", m_GlobalMatrix);

			// Read in the bone count
			uint32_t boneCount = 0;
			root.ReadData("Bone Count", boneCount);

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
				boneNode.ReadData("Inverse Bindpose", bone.InverseBindpose);

				m_Bones[bone.Index] = bone;

				if (bone.ParentIndex == -1)
					m_RootBone = bone.Index;
			}
		}
	}
}