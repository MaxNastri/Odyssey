#pragma once
#include "AssetSerializer.h"
#include "GUID.h"

namespace Odyssey
{
	class SourceAsset
	{
	public:
		SourceAsset() = default;
		SourceAsset(const Path& sourcePath);

	public:
		static SourceAsset CreateFromMetafile(const Path& metaPath);

	public:
		void SerializeMetadata();
		void DeserializeMetadata();

	public:
		GUID GetGUID() { return m_GUID; }
		const std::string& GetName() { return m_Name; }
		const std::string& GetType() { return m_Type; }
		std::filesystem::path& GetPath() { return m_SourcePath; }
		bool HasMetadata();

	public:
		void SetMetadata(GUID guid, const std::string& name, const std::string& type);
		void SetGUID(GUID guid) { m_GUID = guid; }
		void SetName(const std::string& name) { m_Name = name; }
		void SetType(const std::string& type) { m_Type = type; }

	protected: // Serialized
		GUID m_GUID;
		std::string m_Name;
		std::string m_Type;
		std::string m_SourceAsset;
		std::string m_SourceExtension;
		std::filesystem::path m_SourcePath;
		std::filesystem::path m_MetaFilePath;
	};

	class Asset
	{
	public:
		Asset() = default;
		Asset(const Path& assetPath);

	public:
		void SerializeMetadata(AssetSerializer& serializer);
		void Load();

	public:
		GUID GetSoureAsset() { return m_SourceAsset; }
		GUID GetGUID() { return m_GUID; }
		const std::string& GetName() { return m_Name; }
		const std::string& GetType() { return m_Type; }
		Path& GetAssetPath() { return m_AssetPath; }

	public:
		void SetSourceAsset(GUID& guid) { m_SourceAsset = guid; }
		void SetGUID(GUID& guid) { m_GUID = guid; }
		void SetName(const std::string& name) { m_Name = name; }
		void SetType(const std::string& type) { m_Type = type; }

	protected: // Non-serialized
		Path m_AssetPath;

	protected: // Serialized
		GUID m_SourceAsset;
		GUID m_GUID;
		std::string m_Name;
		std::string m_Type;
	};
}