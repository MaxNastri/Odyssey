#pragma once
#include "AssetSerializer.h"
#include "GUID.h"
#include "Ref.h"

namespace Odyssey
{
	class SourceAsset
	{
	public:
		SourceAsset() = default;
		SourceAsset(const Path& sourcePath);

	public:
		void AddOnModifiedListener(std::function<void()> onSourceModified) { m_OnSourceModified.push_back(onSourceModified); }

	public:
		GUID GetGUID() { return Guid; }
		std::string_view GetName() { return Name; }
		std::string_view GetType() { return Type; }
		Path& GetPath() { return m_SourcePath; }
		bool HasMetadata();

	public:
		void SetGUID(GUID guid) { Guid = guid; }
		void SetName(std::string_view name) { Name = name; }
		void SetType(std::string_view type) { Type = type; }
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
		std::vector<std::function<void()>> m_OnSourceModified;
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
		virtual void Save() = 0;

	public:
		GUID GetGUID() { return m_GUID; }
		std::string_view GetName() { return m_Name; }
		std::string_view GetType() { return m_Type; }

	public:
		GUID GetSourceAsset() { return m_SourceAsset; }
		Path& GetAssetPath() { return m_AssetPath; }

	public:
		void SetName(std::string_view name);
		void SetAssetPath(const Path& path);
		void SetSourceAsset(GUID guid) { m_SourceAsset = guid; }

	protected:
		friend class AssetManager;
		GUID m_GUID;
		std::string m_Name;
		std::string m_Type;

	protected:
		Path m_AssetPath;
		GUID m_SourceAsset;
	};
}