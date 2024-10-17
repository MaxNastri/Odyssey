#include "VulkanShaderModule.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include <Log.h>
#include "volk.h"
#include <shaderc/shaderc.hpp>
#include "AssetManager.h"
#include "ShaderCompiler.h"

namespace Odyssey
{
	VulkanShaderModule::VulkanShaderModule(ResourceID id, std::shared_ptr<VulkanContext> context, ShaderType shaderType, BinaryBuffer& codeBuffer)
		: Resource(id)
	{
		m_Context = context;
		m_ShaderType = shaderType;

		CreateShaderModule(codeBuffer);
	}

	void VulkanShaderModule::Destroy()
	{
		vkDestroyShaderModule(m_Context->GetDevice()->GetLogicalDevice(), m_ShaderModule, nullptr);
	}

	VkShaderStageFlagBits VulkanShaderModule::GetShaderFlags()
	{
		switch (m_ShaderType)
		{
			case ShaderType::None:
				break;
			case ShaderType::Fragment:
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			case ShaderType::Vertex:
				return VK_SHADER_STAGE_VERTEX_BIT;
			case ShaderType::Compute:
				return VK_SHADER_STAGE_COMPUTE_BIT;
			case ShaderType::Geometry:
				return VK_SHADER_STAGE_GEOMETRY_BIT;
			default:
				break;
		}

		return VkShaderStageFlagBits();
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
			Log::Error("[VulkanShader] Failed to create shader module.");
		}
	}
}