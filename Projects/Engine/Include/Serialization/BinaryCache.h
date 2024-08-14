#pragma once
#include "BinaryBuffer.h"
#include "GUID.h"

namespace Odyssey
{
	class BinaryCache
	{
	public:
		BinaryCache() = default;
		BinaryCache(const std::filesystem::path& cacheDirectory);

	public:
		BinaryBuffer LoadBinaryData(GUID guid);
		void SaveBinaryData(GUID& guid, BinaryBuffer& buffer);
		void SaveBinaryData(const std::filesystem::path& assetPath, BinaryBuffer& buffer);

	private:
		BinaryBuffer LoadBinaryData(const std::filesystem::path& path);
		std::filesystem::path GenerateAssetPath(GUID guid);

	private:
		std::filesystem::path m_Path;
		std::map<GUID, std::filesystem::path> m_GUIDToPath;
		std::map<std::filesystem::path, GUID> m_PathToGUID;
	};
}