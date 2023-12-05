#include "Shader.h"
#include "ryml.hpp"
#include "VulkanShaderModule.h"
#include "ResourceManager.h"

namespace Odyssey
{
	Shader::Shader(const std::string& filename)
	{
		Load(filename);
		m_ShaderModule = ResourceManager::AllocateShaderModule(m_ShaderType, filename);
	}

	void Shader::Load(const std::string& path)
	{
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;
		
		root["UUID"] << m_UUID;
		root["Name"] << m_Name;
		root["Path"] << m_Path;
		root["Type"] << m_Type;

		root["Shader Type"] << (uint32_t)m_ShaderType;

		FILE* file = fopen(path.c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file);
		fclose(file);
	}

	void Shader::Save(const std::string& path)
	{
		if (std::ifstream ifs{ path })
		{
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			uint32_t shaderType = 0;

			node["UUID"] >> m_UUID;
			node["Name"] >> m_Name;
			node["Path"] >> m_Path;
			node["Type"] >> m_Type;

			node["Shader Type"] >> shaderType;

			m_ShaderType = (ShaderType)shaderType;
		}
	}
}