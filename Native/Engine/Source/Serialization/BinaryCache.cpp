#include "BinaryCache.h"
#include "Logger.h"

namespace Odyssey
{
	BinaryCache::BinaryCache(const std::filesystem::path& cacheDirectory)
	{
		m_Path = cacheDirectory / "BinaryAssets";

		if (!std::filesystem::exists(m_Path))
			std::filesystem::create_directories(m_Path);

		// Init the database
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_Path))
		{
			auto assetPath = dirEntry.path();
			auto extension = assetPath.extension();

			if (dirEntry.is_regular_file() && extension == ".asset")
			{
				// Read contents of a file
				if (LoadBinaryData(assetPath))
				{
					// We use the filename as the guid
					auto guid = assetPath.filename().replace_extension("").string();

					if (!m_GUIDToPath.contains(guid))
					{
						m_GUIDToPath[guid] = assetPath;
						m_PathToGUID[assetPath] = guid;
					}
				}
			}
		}
	}
	BinaryBuffer BinaryCache::LoadBinaryData(const std::string& guid)
	{
		if (m_GUIDToPath.contains(guid))
			return LoadBinaryData(m_GUIDToPath[guid]);
		
		return BinaryBuffer();
	}

	void BinaryCache::SaveBinaryData(const std::filesystem::path& assetPath, BinaryBuffer& buffer)
	{
		std::ofstream file(assetPath, std::ios::trunc | std::ios::binary);
		if (!file.is_open())
		{
			Logger::LogError("[VulkanShader] Unable to open shader file: " + assetPath.string());
			return;
		}

		// Write the buffer size
		size_t bufferSize = buffer.GetSize();
		file.write((char*)&bufferSize, sizeof(size_t));

		// Create the buffer and read into it
		std::vector<unsigned char> bufferData = buffer.GetData();
		file.write((char*)bufferData.data(), bufferSize);

		// Done reading
		file.close();
	}

	void BinaryCache::SaveBinaryData(const std::string& guid, BinaryBuffer& buffer)
	{
		SaveBinaryData(GenerateAssetPath(guid), buffer);
	}

	BinaryBuffer BinaryCache::LoadBinaryData(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
		{
			Logger::LogError("[VulkanShader] Unable to open shader file: " + path.string());
			return BinaryBuffer();
		}

		// Read the buffer size
		size_t bufferSize = 0;
		file.read((char*)&bufferSize, sizeof(size_t));

		// Create the buffer and read into it
		std::vector<unsigned char> buffer(bufferSize);
		file.read((char*)buffer.data(), bufferSize);

		// Done reading
		file.close();

		return BinaryBuffer(buffer, bufferSize);
	}

	std::filesystem::path BinaryCache::GenerateAssetPath(const std::string& guid)
	{
		return m_Path / std::string(guid + ".asset");
	}
}