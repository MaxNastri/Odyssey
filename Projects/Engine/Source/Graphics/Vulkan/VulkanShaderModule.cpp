#include "VulkanShaderModule.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include <Logger.h>
#include "volk.h"
#include <shaderc/shaderc.hpp>
#include "AssetManager.h"
#include "ShaderCompiler.h"

namespace Odyssey
{
	VulkanShaderModule::VulkanShaderModule(std::shared_ptr<VulkanContext> context, ShaderType shaderType, BinaryBuffer& codeBuffer)
	{
		m_Context = context;
		m_ShaderType = shaderType;

		CreateShaderModule(codeBuffer);
	}

	void VulkanShaderModule::Destroy()
	{
		vkDestroyShaderModule(m_Context->GetDevice()->GetLogicalDevice(), m_ShaderModule, nullptr);
	}

	void VulkanShaderModule::CreateShaderModule(BinaryBuffer& codeBuffer)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = codeBuffer.GetSize();
		const unsigned char* bufferPtr = codeBuffer.GetData().data();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(bufferPtr);

		if (vkCreateShaderModule(m_Context->GetDevice()->GetLogicalDevice(), &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS)
		{
			Logger::LogError("[VulkanShader] Failed to create shader module.");
		}
	}
}