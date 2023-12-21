#pragma once

namespace Odyssey
{
	class AssetMetadata
	{
	public:
		AssetMetadata() = default;
		AssetMetadata(const std::filesystem::path& metaPath);

	public:
		void Save();
		void Load();

	public:
		const std::string& GetGUID() { return m_GUID; }
		const std::string& GetName() { return m_Name; }
		const std::string& GetType() { return m_Type; }

	public:
		void SetGUID(const std::string& guid) { m_GUID = guid; }
		void SetName(const std::string& name) { m_Name = name; }
		void SetType(const std::string& type) { m_Type = type; }

	private:
		std::filesystem::path m_FilePath;

	private: // Serialized
		std::string m_GUID;
		std::string m_Name;
		std::string m_Type;
	};
}



// LoadTexture(path):
	// Find the meta file for that path
	// Texture(path)
		// PNG data, not asset data
		// To fill out the asset data, we load via metadata

