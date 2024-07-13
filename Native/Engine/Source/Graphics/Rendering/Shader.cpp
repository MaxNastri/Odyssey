#include "Shader.h"
#include "ryml.hpp"
#include "VulkanShaderModule.h"
#include "ResourceManager.h"

namespace Odyssey
{
	Shader::Shader(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath)
		: Asset(assetPath, metaPath)
	{
		LoadFromDisk(assetPath);

		if (!m_ModulePath.empty())
			m_ShaderModule = ResourceManager::AllocateShaderModule(m_ShaderType, m_ModulePath);
	}

	void Shader::LoadFromDisk(const std::filesystem::path& path)
	{
		if (std::ifstream ifs{ path })
		{
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			uint32_t shaderType = 0;
			std::string modulePath;

			node["m_ShaderType"] >> shaderType;
			node["m_ModulePath"] >> modulePath;

			m_ShaderType = (ShaderType)shaderType;
			m_ModulePath = modulePath;
		}
	}

	void Shader::Save()
	{
		SaveMetadata();
		SaveToDisk(m_AssetPath);
	}

	void Shader::Load()
	{
		LoadMetadata();
		LoadFromDisk(m_AssetPath);
	}

	void Shader::SaveToDisk(const std::filesystem::path& path)
	{
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		root["m_ShaderType"] << (uint32_t)m_ShaderType;
		root["m_ModulePath"] << m_ModulePath.c_str();

		FILE* file = fopen(path.string().c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file);
		fclose(file);
	}
}