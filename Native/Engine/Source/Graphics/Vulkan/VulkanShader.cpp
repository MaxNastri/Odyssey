#include "VulkanShader.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include <Logger.h>
#include "volk.h"

namespace Odyssey
{
	VulkanShader::VulkanShader(std::shared_ptr<VulkanContext> context, ShaderType shaderType, const std::string& filename)
	{
		m_Context = context;
		m_ShaderType = shaderType;
		m_Filename = filename;

		std::vector<char> shaderCode = ReadShaderCode(filename);
		CreateShaderModule(shaderCode);
	}

	void VulkanShader::Destroy()
	{
		vkDestroyShaderModule(m_Context->GetDevice()->GetLogicalDevice(), m_ShaderModule, nullptr);
	}

	std::vector<char> VulkanShader::ReadShaderCode(const std::string& filename)
	{
		std::vector<char> buffer;

		// Open the file as binary
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			Logger::LogError("[VulkanShader] Unable to open shader file: " + filename);
			return buffer;
		}

		// Resized the buffer to the file size
		size_t fileSize = (size_t)file.tellg();
		buffer.resize(fileSize);

		// Read from the beginning into the buffer
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void VulkanShader::CreateShaderModule(std::vector<char> shaderCode)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		if (vkCreateShaderModule(m_Context->GetDevice()->GetLogicalDevice(), &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS)
		{
			Logger::LogError("[VulkanShader] Failed to create shader module.");
		}
	}
}