#include "VulkanTexture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vulkan/vulkan.h>
#include <Logger.h>

namespace Odyssey
{
	VulkanTexture2D::VulkanTexture2D(std::string_view filename)
	{

	}

	void VulkanTexture2D::LoadFromFile(std::string_view filename)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load("../Assets/textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
	}
}