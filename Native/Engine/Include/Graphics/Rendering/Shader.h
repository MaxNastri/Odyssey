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
		Shader(const std::string& filename);

	public:
		ResourceHandle<VulkanShaderModule> GetShaderModule() { return m_ShaderModule; }

	public:
		void Load(const std::string& path);
		void Save();
		void SaveTo(const std::string& path);

	private:
		ShaderType m_ShaderType;
		std::string m_ModulePath;
		ResourceHandle<VulkanShaderModule> m_ShaderModule;
	};
}