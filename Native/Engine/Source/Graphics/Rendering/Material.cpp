#include "Material.h"
#include "Shader.h"
#include "AssetManager.h"
#include "ryml.hpp"

namespace Odyssey
{
	Material::Material(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath)
		: Asset(assetPath, metaPath)
	{
		LoadFromDisk(assetPath);
	}

	void Material::Save()
	{
		if (!m_AssetPath.empty())
		{
			SaveMetadata();
			SaveToDisk(m_AssetPath);
		}
	}

	void Material::Load()
	{
		if (!m_AssetPath.empty())
		{
			LoadMetadata();
			LoadFromDisk(m_AssetPath);
		}
	}

	void Material::SaveToDisk(const std::filesystem::path& assetPath)
	{
		// Create a tree and root node
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		root["m_FragmentShader"] << m_FragmentShader.Get()->GetGUID();
		root["m_VertexShader"] << m_VertexShader.Get()->GetGUID();

		// Save to disk
		FILE* file2 = fopen(assetPath.string().c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file2);
		fclose(file2);
	}

	void Material::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		if (std::ifstream ifs{ assetPath })
		{
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			std::string fragGUID;
			std::string vertGUID;
			node["m_FragmentShader"] >> fragGUID;
			node["m_VertexShader"] >> vertGUID;

			m_FragmentShader = AssetManager::LoadShaderByGUID(fragGUID);
			m_VertexShader = AssetManager::LoadShaderByGUID(vertGUID);
		}
	}
}