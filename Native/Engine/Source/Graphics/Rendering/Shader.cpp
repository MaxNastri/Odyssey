#include "Shader.h"
#include "VulkanShaderModule.h"
#include "ResourceManager.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	Shader::Shader(const std::filesystem::path& assetPath)
		: Asset(assetPath)
	{
		LoadFromDisk(assetPath);

		if (!m_ModulePath.empty())
			m_ShaderModule = ResourceManager::AllocateShaderModule(m_ShaderType, m_ModulePath);
	}

	void Shader::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			uint32_t shaderType = 0;
			std::string modulePath;

			root.ReadData("m_ShaderType", shaderType);
			root.ReadData("m_ModulePath", modulePath);

			m_ShaderType = (ShaderType)shaderType;
			m_ModulePath = modulePath;

		}
	}

	void Shader::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Shader::Load()
	{
		LoadFromDisk(m_AssetPath);
	}

	void Shader::SaveToDisk(const std::filesystem::path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize metadata first
		SerializeMetadata(serializer);

		root.WriteData("m_ShaderType", (uint32_t)m_ShaderType);
		root.WriteData("m_ModulePath", m_ModulePath.c_str());
		serializer.WriteToDisk(path);
	}
}