#include "VulkanRenderTexture.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "ResourceManager.h"
#include <Logger.h>
#include "volk.h"
#include "VulkanImage.h"

#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"

namespace Odyssey
{
	//VulkanTexture::VulkanTexture(std::shared_ptr<VulkanContext> context, const std::string& filename)
	//{
	//	m_Context = context;
	//	m_Sampler = std::make_unique<VulkanTextureSampler>(context);

	//	// Load the texture via stb
	//	int texWidth, texHeight, texChannels;
	//	stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	//	if (!pixels)
	//	{
	//		Logger::LogError("[VulkanTexture] Unable to load texture file " + filename);
	//		return;
	//	}

	//	// Create a staging buffer to copy the pixel data into vulkanized memory
	//	uint32_t imageSize = (uint32_t)(texWidth * texHeight * 4);
	//	m_StagingBuffer = ResourceManager::AllocateBuffer(BufferType::Staging, imageSize);
	//	m_StagingBuffer.Get()->SetMemory(imageSize, pixels);

	//	// Now that we have transfered the memory, free the original pixel data
	//	stbi_image_free(pixels);

	//	m_Width = texWidth;
	//	m_Height = texHeight;

	//	VulkanImageDescription imageDesc;
	//	imageDesc.Width = texWidth;
	//	imageDesc.Height = texHeight;
	//	m_Image = std::make_unique<VulkanImage>(context, imageDesc);

	//	ResourceHandle<VulkanCommandPool> poolHandle = m_Context->GetCommandPool();
	//	ResourceHandle<VulkanCommandBuffer> bufferHandle = poolHandle.Get()->AllocateBuffer();
	//	VulkanCommandBuffer* commandBuffer = bufferHandle.Get();

	//	// Transition layouts so we can transfer data
	//	{
	//		commandBuffer->BeginCommands();
	//		commandBuffer->TransitionLayouts(m_Image.get(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	//		commandBuffer->EndCommands();

	//		m_Context->SubmitCommandBuffer(bufferHandle);
	//	}
	//	
	//	// Set the data from the staging buffer
	//	m_Image->SetData(m_StagingBuffer, texWidth, texHeight);
	//	
	//	// Reset the command buffer
	//	commandBuffer->Reset();

	//	// Transition layouts into a gpu-read-only format
	//	{
	//		commandBuffer->BeginCommands();
	//		commandBuffer->TransitionLayouts(m_Image.get(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	//		commandBuffer->EndCommands();

	//		m_Context->SubmitCommandBuffer(bufferHandle);
	//	}

	//	poolHandle.Get()->ReleaseBuffer(bufferHandle);
	//}

	VulkanRenderTexture::VulkanRenderTexture(std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height)
	{
		m_Context = context;
		m_Width = width;
		m_Height = height;

		VulkanImageDescription imageDesc;
		imageDesc.Width = width;
		imageDesc.Height = height;
		imageDesc.ImageType = TextureType::RenderTexture;

		m_Image = ResourceManager::AllocateImage(imageDesc);

		auto pool = context->GetCommandPool();
		ResourceHandle<VulkanCommandBuffer> commandBufferHandle = pool.Get()->AllocateBuffer();

		if (VulkanCommandBuffer* commandBuffer = commandBufferHandle.Get())
		{
			commandBuffer->BeginCommands();
			commandBuffer->TransitionLayouts(m_Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			commandBuffer->EndCommands();
			commandBuffer->Flush();
		}
		pool.Get()->ReleaseBuffer(commandBufferHandle);
	}

	VulkanRenderTexture::VulkanRenderTexture(std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height, TextureFormat format)
	{
		m_Context = context;
		m_Width = width;
		m_Height = height;

		VulkanImageDescription imageDesc;
		imageDesc.Width = width;
		imageDesc.Height = height;
		imageDesc.Format = format;
		imageDesc.ImageType = IsDepthTexture(format) ? TextureType::DepthTexture : TextureType::RenderTexture;
		m_Image = ResourceManager::AllocateImage(imageDesc);

		auto pool = context->GetCommandPool();
		ResourceHandle<VulkanCommandBuffer> commandBufferHandle = pool.Get()->AllocateBuffer();

		if (VulkanCommandBuffer* commandBuffer = commandBufferHandle.Get())
		{
			commandBuffer->BeginCommands();
			commandBuffer->TransitionLayouts(m_Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			commandBuffer->EndCommands();
			commandBuffer->Flush();
		}
		pool.Get()->ReleaseBuffer(commandBufferHandle);
	}

	VulkanRenderTexture::VulkanRenderTexture(std::shared_ptr<VulkanContext> context, ResourceHandle<VulkanImage> image, TextureFormat format)
	{
		m_Context = context;
		m_Width = image.Get()->GetWidth();
		m_Height = image.Get()->GetHeight();
		m_Image = image;
	}

	void VulkanRenderTexture::Destroy()
	{
		ResourceManager::DestroyImage(m_Image);
	}

	void VulkanRenderTexture::SetData(const void* data)
	{
		if (VulkanImage* image = m_Image.Get())
			image->SetData(data);
	}

	bool VulkanRenderTexture::IsDepthTexture(TextureFormat format)
	{
		return format == TextureFormat::D32_SFLOAT || format == TextureFormat::D32_SFLOAT_S8_UINT ||
			format == TextureFormat::D24_UNORM_S8_UINT;
	}
}