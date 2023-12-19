#include "Material.h"
#include "Shader.h"
#include "AssetManager.h"
#include "ryml.hpp"

namespace Odyssey
{
	Material::Material(const std::string& assetPath)
	{
		Load(assetPath);
	}

	void Material::Save()
	{
		if (!m_AssetPath.empty())
			SaveTo(m_AssetPath);
	}

	void Material::SaveTo(const std::string& assetPath)
	{
		// Create a tree and root node
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		// Serialize the base asset data
		root["m_GUID"] << m_GUID;
		root["m_Name"] << m_Name;
		root["m_AssetPath"] << assetPath;
		root["m_Type"] << m_Type;

		root["m_FragmentShader"] << m_FragmentShader.Get()->GetGUID();
		root["m_VertexShader"] << m_VertexShader.Get()->GetGUID();

		// Save to disk
		FILE* file2 = fopen(assetPath.c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file2);
		fclose(file2);
	}

	void Material::Load(const std::string& assetPath)
	{
		if (std::ifstream ifs{ assetPath })
		{
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			node["m_GUID"] >> m_GUID;
			node["m_Name"] >> m_Name;
			node["m_AssetPath"] >> m_AssetPath;
			node["m_Type"] >> m_Type;

			std::string fragGUID;
			std::string vertGUID;
			node["m_FragmentShader"] >> fragGUID;
			node["m_VertexShader"] >> vertGUID;

			m_FragmentShader = AssetManager::LoadShaderByGUID(fragGUID);
			m_VertexShader = AssetManager::LoadShaderByGUID(vertGUID);
		}
	}
}