#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkShaderModule)

namespace Odyssey
{
	class VulkanContext;

	enum class ShaderType
	{
		None = 0,
		Fragment = 1,
		Vertex = 2,
	};
	class VulkanShader
	{
	public:
		VulkanShader(std::shared_ptr<VulkanContext> context, ShaderType shaderType, const std::string& filename);
		void Destroy();

	public:
		VkShaderModule GetShaderModule() { return m_ShaderModule; }
		ShaderType GetShaderType() { return m_ShaderType; }

	private:
		std::vector<char> ReadShaderCode(const std::string& filename);
		void CreateShaderModule(std::vector<char> shaderCode);

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkShaderModule m_ShaderModule;
		ShaderType m_ShaderType;
		std::string m_Filename;
	};
}