#include "VulkanDescriptorBuffer.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorLayout.h"
#include "ResourceManager.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanTextureSampler.h"

namespace Odyssey
{
	inline VkDeviceSize AlignedSize(VkDeviceSize value, VkDeviceSize alignment)
	{
		return (value + alignment - 1) & ~(alignment - 1);
	}

	VulkanDescriptorBuffer::VulkanDescriptorBuffer(std::shared_ptr<VulkanContext> context, ResourceHandle<VulkanDescriptorLayout> layout, uint32_t count)
	{
		m_Context = context;
		m_Layout = layout;
		m_Count = count;

		bool isUniformBuffer = m_Layout.Get()->GetType() == DescriptorType::Uniform;

		VkPhysicalDeviceDescriptorBufferPropertiesEXT descriptor_buffer_properties{};
		VkPhysicalDeviceProperties2KHR device_properties{};

		descriptor_buffer_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT;
		device_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
		device_properties.pNext = &descriptor_buffer_properties;
		vkGetPhysicalDeviceProperties2KHR(context->GetPhysicalDeviceVK(), &device_properties);

		m_Properties.offsetAlignment = descriptor_buffer_properties.descriptorBufferOffsetAlignment;

		if (isUniformBuffer)
			m_Properties.uniformBufferDescriptorSize = descriptor_buffer_properties.uniformBufferDescriptorSize;
		else
			m_Properties.uniformBufferDescriptorSize = descriptor_buffer_properties.samplerDescriptorSize;

		VkDeviceSize size;
		vkGetDescriptorSetLayoutSizeEXT(m_Context->GetDeviceVK(), m_Layout.Get()->GetHandle(), &size);
		size = AlignedSize(size, m_Properties.offsetAlignment);

		VkDeviceSize offset;
		vkGetDescriptorSetLayoutBindingOffsetEXT(m_Context->GetDeviceVK(), m_Layout.Get()->GetHandle(), 0, &offset);

		// Store the size and offset
		m_Size = (uint32_t)size;
		m_Offset = (uint32_t)offset;

		// Prepare descriptor buffer
		BufferType bufferType = m_Layout.Get()->GetType() == DescriptorType::Uniform ? BufferType::DescriptorUniform : BufferType::DescriptorSampler;

		m_Buffer = ResourceManager::AllocateBuffer(bufferType, m_Size * count);
		m_Buffer.Get()->AllocateMemory();
	}

	void VulkanDescriptorBuffer::Destroy()
	{

	}

	void VulkanDescriptorBuffer::SetUniformBuffer(ResourceHandle<VulkanBuffer> uniformBuffer, uint32_t index)
	{
		// Get our mapped memory address
		char* uniformDescriptorBuffer = (char*)m_Buffer.Get()->GetMappedMemory();

		VkDescriptorGetInfoEXT descriptorInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT };
		descriptorInfo.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		// Get the address and size of the uniform buffer
		VkDescriptorAddressInfoEXT addressInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT };
		addressInfo.address = uniformBuffer.Get()->GetAddress();
		addressInfo.range = uniformBuffer.Get()->GetSize();
		addressInfo.format = VK_FORMAT_UNDEFINED;

		descriptorInfo.data.pUniformBuffer = &addressInfo;

		char* descriptor = uniformDescriptorBuffer + (index * m_Size) + m_Offset;
		vkGetDescriptorEXT(m_Context->GetDeviceVK(), &descriptorInfo, m_Properties.uniformBufferDescriptorSize, descriptor);
	}

	void VulkanDescriptorBuffer::SetTexture(ResourceHandle<VulkanImage> image, ResourceHandle<VulkanTextureSampler> sampler, uint32_t index)
	{
		// Get our mapped memory address
		char* uniformDescriptorBuffer = (char*)m_Buffer.Get()->GetMappedMemory();

		VkDescriptorGetInfoEXT descriptorInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT };
		descriptorInfo.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = image.Get()->GetImageView();
		imageInfo.sampler = sampler.Get()->GetSamplerVK();

		descriptorInfo.data.pCombinedImageSampler = &imageInfo;

		char* descriptor = uniformDescriptorBuffer + (index * m_Size) + m_Offset;
		vkGetDescriptorEXT(m_Context->GetDeviceVK(), &descriptorInfo, m_Properties.uniformBufferDescriptorSize, descriptor);
	}
}