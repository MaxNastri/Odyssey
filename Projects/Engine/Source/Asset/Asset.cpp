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
		DeserializeMetadata();
	}

	SourceAsset SourceAsset::CreateFromMetafile(const Path& metaPath)
	{
		if (metaPath.extension() != ".meta")
			return SourceAsset();

		std::string sourceExtension;

		// Check if the file exists and can be deserialized
		if (AssetDeserializer deserializer = AssetDeserializer(metaPath))
		{
			SerializationNode root = deserializer.GetRoot();
			root.ReadData("m_SourceExtension", sourceExtension);
			auto sourcePath = metaPath;
			sourcePath = sourcePath.replace_extension(sourceExtension);
			return SourceAsset(sourcePath);
		}

		return SourceAsset();
	}

	void SourceAsset::SerializeMetadata()
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		root.WriteData("m_GUID", m_GUID.CRef());
		root.WriteData("m_Name", m_Name);
		root.WriteData("m_Type", m_Type);
		root.WriteData("m_SourceExtension", m_SourceExtension);

		serializer.WriteToDisk(m_MetaFilePath);
	}

	void SourceAsset::DeserializeMetadata()
	{
		if (m_MetaFilePath.empty())
			return;

		if (AssetDeserializer deserializer = AssetDeserializer(m_MetaFilePath))
		{
			SerializationNode root = deserializer.GetRoot();
			root.ReadData("m_GUID", m_GUID.Ref());
			root.ReadData("m_Name", m_Name);
			root.ReadData("m_Type", m_Type);
			root.ReadData("m_SourceExtension", m_SourceExtension);
		}
	}

	bool SourceAsset::HasMetadata()
	{
		return m_GUID != 0 && !m_Name.empty() && !m_Type.empty() && !m_SourceExtension.empty();
	}

	void SourceAsset::SetMetadata(GUID guid, const std::string& name, const std::string& type)
	{
		m_GUID = guid;
		m_Name = name;
		m_Type = type;
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
		root.WriteData("m_GUID", m_GUID.CRef());
		root.WriteData("m_Name", m_Name);
		root.WriteData("m_Type", m_Type);
	}

	void Asset::Load()
	{
		AssetDeserializer deserializer(m_AssetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			root.ReadData("m_SourceAsset", m_SourceAsset.Ref());
			root.ReadData("m_GUID", m_GUID.Ref());
			root.ReadData("m_Name", m_Name);
			root.ReadData("m_Type", m_Type);
		}
	}
}