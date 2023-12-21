#include "AssetMetadata.h"
#include "ryml.hpp"

namespace Odyssey
{
	AssetMetadata::AssetMetadata(const std::filesystem::path& metaPath)
	{
		m_FilePath = metaPath;
		Load();
	}

	void AssetMetadata::Save()
	{
		// Create a tree and root node
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		// Serialize the base asset data
		root["m_GUID"] << m_GUID;
		root["m_Name"] << m_Name;
		root["m_Type"] << m_Type;

		// Save to disk
		FILE* file2 = fopen(m_FilePath.string().c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file2);
		fclose(file2);
	}

	void AssetMetadata::Load()
	{
		if (std::ifstream ifs{ m_FilePath })
		{
			// Create the yaml root node
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			node["m_GUID"] >> m_GUID;
			node["m_Name"] >> m_Name;
			node["m_Type"] >> m_Type;

			ifs.close();
		}
	}
}