#include "VulkanTexture.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include <Logger.h>
#include <vulkan/vulkan.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Odyssey
{
	VulkanTexture::VulkanTexture(std::shared_ptr<VulkanContext> context, const std::string& filename)
	{
		m_Context = context;
		m_Sampler = std::make_unique<VulkanTextureSampler>(context);

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
		ResourceHandle<VulkanBuffer> stagingBuffer = ResourceManager::AllocateBuffer(BufferType::Staging, imageSize);
		stagingBuffer.Get()->SetMemory(imageSize, pixels);

		// Now that we have transfered the memory, free the original pixel data
		stbi_image_free(pixels);

		VulkanImageDescription imageDesc;
		imageDesc.Width = texWidth;
		imageDesc.Height = texHeight;
		m_Image = std::make_unique<VulkanImage>(context, imageDesc);

		VulkanCommandPool* commandPool = m_Context->GetCommandPool();
		VulkanCommandBuffer* commandBuffer = commandPool->AllocateBuffer();

		// Transition layouts so we can transfer data
		{
			commandBuffer->BeginCommands();
			commandBuffer->TransitionLayouts(m_Image.get(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			commandBuffer->EndCommands();

			m_Context->SubmitCommandBuffer(commandBuffer);
		}
		
		// Set the data from the staging buffer
		m_Image->SetData(stagingBuffer, texWidth, texHeight);
		
		// Reset the command buffer
		commandBuffer->Reset();

		// Transition layouts into a gpu-read-only format
		{
			commandBuffer->BeginCommands();
			commandBuffer->TransitionLayouts(m_Image.get(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			commandBuffer->EndCommands();

			m_Context->SubmitCommandBuffer(commandBuffer);
		}

		commandPool->ReleaseBuffer(commandBuffer);
		ResourceManager::DestroyBuffer(stagingBuffer);
	}

	VulkanTexture::VulkanTexture(std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height)
	{
		VulkanImageDescription imageDesc;
		imageDesc.Width = width;
		imageDesc.Height = height;
		imageDesc.ImageType = ImageType::RenderTexture;

		m_Context = context;
		m_Image = std::make_unique<VulkanImage>(context, imageDesc);
		m_Sampler = std::make_unique<VulkanTextureSampler>(context);
	}

	void VulkanTexture::Destroy()
	{
		m_Image->Destroy();
		m_Sampler->Destroy();
	}
}