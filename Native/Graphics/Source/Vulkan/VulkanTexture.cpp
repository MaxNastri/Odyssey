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
		VkBufferUsageFlags flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		VulkanBuffer stagingBuffer(context, imageSize, flags, properties);
		stagingBuffer.SetMemory(imageSize, pixels);

		// Now that we have transfered the memory, free the original pixel data
		stbi_image_free(pixels);


	}
}