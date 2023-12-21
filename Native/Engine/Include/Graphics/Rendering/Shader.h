#pragma once
#include "Asset.h"
#include "ResourceHandle.h"
#include "Enums.h"

namespace Odyssey
{
	class VulkanShaderModule;

	class Shader : public Asset
	{
	public:
		Shader() = default;
		Shader(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath);

	public:
		ResourceHandle<VulkanShaderModule> GetShaderModule() { return m_ShaderModule; }

	public:
		void Save();
		void Load();

	private:
		void LoadFromDisk(const std::filesystem::path& path);
		void SaveToDisk(const std::filesystem::path& path);

	private:
		ShaderType m_ShaderType;
		std::filesystem::path m_ModulePath;
		ResourceHandle<VulkanShaderModule> m_ShaderModule;
	};
}