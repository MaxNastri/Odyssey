#pragma once
#include "BinaryBuffer.h"

namespace Odyssey
{
	class BinaryCache
	{
	public:
		BinaryCache();

	public:
		BinaryBuffer LoadBinaryData(const std::string& guid);
		void SaveBinaryData(const std::string& guid, BinaryBuffer& buffer);
		void SaveBinaryData(const std::filesystem::path& assetPath, BinaryBuffer& buffer);

	private:
		BinaryBuffer LoadBinaryData(const std::filesystem::path& path);
		std::filesystem::path GenerateAssetPath(const std::string& guid);

	private:
		std::map<std::string, std::filesystem::path> m_GUIDToPath;
		std::map<std::filesystem::path, std::string> m_PathToGUID;
	};
}