#pragma once
#include "AssetSerializer.h"

namespace Odyssey
{
	class SourceAsset
	{
	public:
		SourceAsset() = default;
		SourceAsset(const std::filesystem::path& sourcePath);

	public:
		static SourceAsset CreateFromMetafile(const std::filesystem::path& metaPath);

	public:
		void SerializeMetadata();
		void DeserializeMetadata();

	public:
		const std::string& GetGUID() { return m_GUID; }
		const std::string& GetName() { return m_Name; }
		const std::string& GetType() { return m_Type; }
		std::filesystem::path& GetPath() { return m_SourcePath; }
		bool HasMetadata();

	public:
		void SetMetadata(const std::string& guid, const std::string& name, const std::string& type);
		void SetGUID(const std::string& guid) { m_GUID = guid; }
		void SetName(const std::string& name) { m_Name = name; }
		void SetType(const std::string& type) { m_Type = type; }

	protected: // Serialized
		std::string m_GUID;
		std::string m_Name;
		std::string m_Type;
		std::string m_SourceExtension;
		std::filesystem::path m_SourcePath;
		std::filesystem::path m_MetaFilePath;
	};

	class Asset
	{
	public:
		Asset() = default;
		Asset(const std::filesystem::path& assetPath);

	public:
		void SerializeMetadata(AssetSerializer& serializer);
		void Load();

	public:
		const std::string& GetGUID() { return m_GUID; }
		const std::string& GetName() { return m_Name; }
		const std::string& GetType() { return m_Type; }
		std::filesystem::path& GetAssetPath() { return m_AssetPath; }

	public:
		void SetGUID(const std::string& guid) { m_GUID = guid; }
		void SetName(const std::string& name) { m_Name = name; }
		void SetType(const std::string& type) { m_Type = type; }

	protected: // Non-serialized
		std::filesystem::path m_AssetPath;

	protected: // Serialized
		std::string m_GUID;
		std::string m_Name;
		std::string m_Type;
	};
}