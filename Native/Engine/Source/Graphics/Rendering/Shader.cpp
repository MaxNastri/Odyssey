#include "Shader.h"
#include "ryml.hpp"
#include "VulkanShaderModule.h"
#include "ResourceManager.h"

namespace Odyssey
{
	Shader::Shader(const std::string& assetPath)
	{
		Load(assetPath);
		m_ShaderModule = ResourceManager::AllocateShaderModule(m_ShaderType, m_ModulePath);
	}

	void Shader::Load(const std::string& path)
	{
		if (std::ifstream ifs{ path })
		{
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			uint32_t shaderType = 0;

			node["m_UUID"] >> m_UUID;
			node["m_Name"] >> m_Name;
			node["m_AssetPath"] >> m_AssetPath;
			node["m_Type"] >> m_Type;

			node["m_ShaderType"] >> shaderType;
			node["m_ModulePath"] >> m_ModulePath;
			m_ShaderType = (ShaderType)shaderType;
		}
	}

	void Shader::Save(const std::string& path)
	{
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		root["m_UUID"] << m_UUID;
		root["m_Name"] << m_Name;
		root["m_AssetPath"] << m_AssetPath;
		root["m_Type"] << m_Type;

		root["m_ShaderType"] << (uint32_t)m_ShaderType;
		root["m_ModulePath"] << m_ModulePath;

		FILE* file = fopen(path.c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file);
		fclose(file);
	}
}