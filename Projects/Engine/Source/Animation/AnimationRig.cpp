#include "AnimationRig.h"
#include "SourceModel.h"
#include "AssetManager.h"

namespace Odyssey
{
	AnimationRig::AnimationRig(const Path& assetPath)
		: Asset(assetPath)
	{
		if (auto source = AssetManager::LoadSourceAsset<SourceModel>(m_SourceAsset))
		{
			source->AddOnModifiedListener([this]() { OnSourceModified(); });
			LoadFromSource(source);
		}
	}

	AnimationRig::AnimationRig(const Path& assetPath, std::shared_ptr<SourceModel> source)
		: Asset(assetPath)
	{
		source->AddOnModifiedListener([this]() { OnSourceModified(); });

		SetSourceAsset(source->GetGUID());
		LoadFromSource(source);
	}

	void AnimationRig::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void AnimationRig::Load()
	{
		if (auto source = AssetManager::LoadSourceAsset<SourceModel>(m_SourceAsset))
			LoadFromSource(source);
	}

	void AnimationRig::LoadFromSource(std::shared_ptr<SourceModel> source)
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

			if (bone.ParentIndex == -1)
				m_RootBone = bone.Index;
		}
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

	void AnimationRig::OnSourceModified()
	{
		Load();
	}
}