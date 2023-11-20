#include "VulkanTexture.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include <Logger.h>
#include <vulkan/vulkan.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Odyssey
{
	VulkanTexture::VulkanTexture(std::shared_ptr<VulkanContext> context, const std::string& filename)
	{
		m_Context = context;

		// Load the texture via stb
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (!pixels)
		{
			Logger::LogError("[VulkanTexture] Unable to load texture file " + filename);
			return;
		}

		// Create a staging buffer to copy the pixel data into vulkanized memory
		VkDeviceSize imageSize = texWidth * texHeight * 4;
		VulkanBuffer stagingBuffer(context, BufferType::Staging, imageSize);
		stagingBuffer.SetMemory(imageSize, pixels);

		// Now that we have transfered the memory, free the original pixel data
		stbi_image_free(pixels);

		VulkanImageDescription imageDesc;
		imageDesc.Width = texWidth;
		imageDesc.Height = texHeight;
		image = std::make_unique<VulkanImage>(context, imageDesc);

		image->TransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		image->SetData(&stagingBuffer, texWidth, texHeight);
		image->TransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		stagingBuffer.Destroy();
	}
}