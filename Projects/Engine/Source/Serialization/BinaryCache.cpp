#include "BinaryCache.h"
#include "Log.h"

namespace Odyssey
{
	BinaryCache::BinaryCache(const Path& cacheDirectory)
	{
		m_Path = cacheDirectory / "BinaryAssets";

		if (!std::filesystem::exists(m_Path))
			std::filesystem::create_directories(m_Path);

		TrackingOptions options;
		options.TrackingPath = m_Path;
		options.Extensions = { ".asset" };
		options.Recursive = true;
		options.Callback = [this](const Path& path, FileActionType fileAction) { OnFileAction(path, fileAction); };
		m_FileTracker = std::make_unique<FileTracker>(options);

		CatalogAssets();
	}
	BinaryBuffer BinaryCache::LoadBinaryData(GUID guid)
	{
		if (m_GUIDToPath.contains(guid))
			return LoadBinaryData(m_GUIDToPath[guid]);
		
		return BinaryBuffer();
	}

	void BinaryCache::SaveBinaryData(const Path& assetPath, BinaryBuffer& buffer)
	{
		std::ofstream file(assetPath, std::ios::trunc | std::ios::binary);
		if (!file.is_open())
		{
			Log::Error("[VulkanShader] Unable to open shader file: " + assetPath.string());
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

	void BinaryCache::SaveBinaryData(GUID& guid, BinaryBuffer& buffer)
	{
		SaveBinaryData(GenerateAssetPath(guid), buffer);
	}

	void BinaryCache::CatalogAssets()
	{
		// Clear the lookups before we begin
		m_GUIDToPath.clear();
		m_PathToGUID.clear();

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
					GUID guid = assetPath.filename().replace_extension("").string();
					if (!m_GUIDToPath.contains(guid))
					{
						m_GUIDToPath[guid] = assetPath;
						m_PathToGUID[assetPath] = guid;
					}
				}
			}
		}
	}

	BinaryBuffer BinaryCache::LoadBinaryData(const Path& path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
		{
			Log::Error("[VulkanShader] Unable to open shader file: " + path.string());
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

		return BinaryBuffer(buffer);
	}

	Path BinaryCache::GenerateAssetPath(GUID guid)
	{
		return m_Path / std::string(guid.String() + ".asset");
	}
	void BinaryCache::OnFileAction(const Path& path, FileActionType fileAction)
	{
		if (fileAction != FileActionType::None && fileAction != FileActionType::Modified)
			CatalogAssets();
	}
}