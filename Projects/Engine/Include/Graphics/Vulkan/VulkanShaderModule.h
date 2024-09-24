#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "Enums.h"
#include "BinaryBuffer.h"

VK_FWD_DECLARE(VkShaderModule)

namespace Odyssey
{
	class VulkanContext;

	class VulkanShaderModule : public Resource
	{
	public:
		VulkanShaderModule(std::shared_ptr<VulkanContext> context, ShaderType shaderType, BinaryBuffer& codeBuffer);
		void Destroy();

	public:
		VkShaderModule GetShaderModule() { return m_ShaderModule; }
		ShaderType GetShaderType() { return m_ShaderType; }
		VkShaderStageFlagBits GetShaderFlags();

	private:
		void CreateShaderModule(BinaryBuffer& codeBuffer);

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkShaderModule m_ShaderModule;
		ShaderType m_ShaderType;
		std::filesystem::path m_FilePath;
	};
}