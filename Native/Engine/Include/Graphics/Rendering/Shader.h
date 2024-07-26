#pragma once
#include "Asset.h"
#include "ResourceHandle.h"
#include "Enums.h"
#include "AssetHandle.h"
#include "BinaryBuffer.h"

namespace Odyssey
{
	class VulkanShaderModule;
	class SourceShader;

	class Shader : public Asset
	{
	public:
		Shader() = default;
		Shader(const std::filesystem::path& assetPath);

	public:
		ResourceHandle<VulkanShaderModule> GetShaderModule() { return m_ShaderModule; }
		const std::string& GetShaderCodeGUID() { return m_ShaderCodeGUID; }
		ShaderType GetShaderType() { return m_ShaderType; }

	public:
		void Save();
		void Load();

	private:
		void LoadFromDisk(const std::filesystem::path& path);
		void SaveToDisk(const std::filesystem::path& path);

	private:
		ShaderType m_ShaderType;
		std::string m_ShaderCodeGUID;
		BinaryBuffer m_ShaderCodeBuffer;
		ResourceHandle<VulkanShaderModule> m_ShaderModule;
	};
}