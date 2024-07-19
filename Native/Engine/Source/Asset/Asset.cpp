#include "Asset.h"
#include "ryml.hpp"

namespace Odyssey
{
	Asset::Asset(const std::filesystem::path& assetPath)
	{
		m_AssetPath = assetPath;
		Load();
	}

	void Asset::SerializeMetadata(AssetSerializer& serializer)
	{
		SerializationNode& root = serializer.GetRoot();
		root.WriteData("m_GUID", m_GUID.c_str());
		root.WriteData("m_Name", m_Name.c_str());
		root.WriteData("m_Type", m_Type.c_str());
	}

	void Asset::Load()
	{
		AssetDeserializer deserializer(m_AssetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			root.ReadData("m_GUID", m_GUID);
			root.ReadData("m_Name", m_Name);
			root.ReadData("m_Type", m_Type);
		}
	}
}