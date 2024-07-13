#pragma once
#include "AssetMetadata.h"

namespace Odyssey
{
	class Asset
	{
	public:
		Asset() = default;

		Asset(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath)
		{
			m_AssetPath = assetPath;
			m_Metadata = AssetMetadata(metaPath);
		}

	public:
		const std::string& GetGUID() { return m_Metadata.GetGUID(); }
		const std::string& GetName() { return m_Metadata.GetName(); }
		std::filesystem::path& GetAssetPath() { return m_AssetPath; }

	public:
		void SetGUID(const std::string& guid) { m_Metadata.SetGUID(guid); }
		void SetName(const std::string& name) { m_Metadata.SetName(name); }
		void SetType(const std::string& type) { m_Metadata.SetType(type); }

	public:
		void SaveMetadata() { m_Metadata.Save(); }
		void LoadMetadata() { m_Metadata.Load(); }

	protected: // Non-serialized
		std::filesystem::path m_AssetPath;

	protected:
		AssetMetadata m_Metadata;
	};
}