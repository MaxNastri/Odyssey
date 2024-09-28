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
		void AddOnModifiedListener(std::function<void()> onSourceModified) { m_OnSourceModified.push_back(onSourceModified); }

	public:
		GUID GetGUID() { return Guid; }
		const std::string& GetName() { return Name; }
		const std::string& GetType() { return Type; }
		Path& GetPath() { return m_SourcePath; }
		bool HasMetadata();

	public:
		void SetGUID(GUID guid) { Guid = guid; }
		void SetName(const std::string& name) { Name = name; }
		void SetType(const std::string& type) { Type = type; }
		void SetMetadata(GUID guid, const std::string& name, const std::string& type);

	public:
		GUID Guid;
		std::string Name;
		std::string Type;

	protected:
		void OnSourceModified();

	protected: // Serialized
		std::string m_SourceAsset;
		std::string m_SourceExtension;
		Path m_SourcePath;
		Path m_MetaFilePath;
		std::vector<std::function<void()>> m_OnSourceModified;
	};

	class Asset
	{
	public:
		Asset() = default;
		Asset(const Path& assetPath);

	public:
		GUID GetGUID() { return Guid; }
		const std::string& GetName() { return Name; }
		const std::string& GetType() { return Type; }
		void SerializeMetadata(AssetSerializer& serializer);
		void Load();

	public:
		GUID GetSoureAsset() { return m_SourceAsset; }
		Path& GetAssetPath() { return m_AssetPath; }

	public:
		void SetGUID(GUID guid) { Guid = guid; }
		void SetName(const std::string& name) { Name = name; }
		void SetType(const std::string& type) { Type = type; }
		void SetSourceAsset(GUID guid) { m_SourceAsset = guid; }

	public:
		GUID Guid;
		std::string Name;
		std::string Type;

	protected:
		Path m_AssetPath;
		GUID m_SourceAsset;
	};
}