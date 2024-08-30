#include "AnimationRig.h"

namespace Odyssey
{
	AnimationRig::AnimationRig(const Path& assetPath)
		: Asset(assetPath)
	{
		LoadFromDisk(assetPath);
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



		serializer.WriteToDisk(assetPath);
	}

	void AnimationRig::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
		}
	}
}