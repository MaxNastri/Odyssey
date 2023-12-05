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

	void Material::Load(const std::string& assetPath)
	{

		if (std::ifstream ifs{ assetPath })
		{
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			node["m_UUID"] >> m_UUID;
			node["m_Name"] >> m_Name;
			node["m_AssetPath"] >> m_AssetPath;
			node["m_Type"] >> m_Type;

			std::string fragUUID;
			std::string vertUUID;
			node["m_FragmentShader"] >> fragUUID;
			node["m_VertexShader"] >> vertUUID;

			m_FragmentShader = AssetManager::LoadShaderByUUID(fragUUID);
			m_VertexShader = AssetManager::LoadShaderByUUID(vertUUID);
		}
	}

	void Material::Save(const std::string& m_AssetPath)
	{
		// Create a tree and root node
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		// Serialize the base asset data
		root["m_UUID"] << m_UUID;
		root["m_Name"] << m_Name;
		root["m_AssetPath"] << m_AssetPath;
		root["m_Type"] << m_Type;

		root["m_FragmentShader"] << m_FragmentShader.Get()->GetUUID();
		root["m_VertexShader"] << m_VertexShader.Get()->GetUUID();

		// Save to disk
		FILE* file2 = fopen(m_AssetPath.c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file2);
		fclose(file2);
	}
}