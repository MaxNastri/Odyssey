#include "Asset.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	SourceAsset::SourceAsset(const Path& sourcePath)
	{
		m_SourcePath = sourcePath;
		m_MetaFilePath = sourcePath;
		m_MetaFilePath = m_MetaFilePath.replace_extension(".meta");
		m_SourceExtension = sourcePath.extension().string();
	}

	bool SourceAsset::HasMetadata()
	{
		return Guid != 0 && !Name.empty() && !Type.empty() && !m_SourceExtension.empty();
	}

	void SourceAsset::SetMetadata(GUID guid, const std::string& name, const std::string& type)
	{
		Guid = guid;
		Name = name;
		Type = type;
	}

	void SourceAsset::OnSourceModified()
	{
		for (auto& callback : m_OnSourceModified)
			callback();
	}

	Asset::Asset(const Path& assetPath)
	{
		m_AssetPath = assetPath;
		Load();
	}

	void Asset::SerializeMetadata(AssetSerializer& serializer)
	{
		SerializationNode& root = serializer.GetRoot();
		root.WriteData("m_SourceAsset", m_SourceAsset.CRef());
		root.WriteData("m_GUID", Guid.CRef());
		root.WriteData("m_Name", Name);
		root.WriteData("m_Type", Type);
	}

	void Asset::Load()
	{
		AssetDeserializer deserializer(m_AssetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			root.ReadData("m_SourceAsset", m_SourceAsset.Ref());
			root.ReadData("m_GUID", Guid.Ref());
			root.ReadData("m_Name", Name);
			root.ReadData("m_Type", Type);
		}
	}
}