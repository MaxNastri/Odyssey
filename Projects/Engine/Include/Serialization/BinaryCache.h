#pragma once
#include "BinaryBuffer.h"
#include "GUID.h"
#include "FileTracker.h"

namespace Odyssey
{
	class BinaryCache
	{
	public:
		BinaryCache() = default;
		BinaryCache(const Path& cacheDirectory);

	public:
		BinaryBuffer LoadBinaryData(GUID guid);
		void SaveBinaryData(GUID& guid, BinaryBuffer& buffer);
		void SaveBinaryData(const Path& assetPath, BinaryBuffer& buffer);

	private:
		void CatalogAssets();
		BinaryBuffer LoadBinaryData(const Path& path);
		Path GenerateAssetPath(GUID guid);
		void OnFileAction(const Path& oldFilename, const Path& newFilename, FileActionType fileAction);

	private:
		Path m_Path;
		std::map<GUID, Path> m_GUIDToPath;
		std::map<Path, GUID> m_PathToGUID;
		std::unique_ptr<FileTracker> m_FileTracker = nullptr;
	};
}