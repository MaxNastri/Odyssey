#include "VulkanVertexBuffer.h"
#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"

namespace Odyssey
{
	VulkanVertexBuffer::VulkanVertexBuffer(std::shared_ptr<VulkanContext> context, std::vector<VulkanVertex>& vertices)
	{
		m_Context = context;

		VkDeviceSize dataSize = vertices.size() * sizeof(vertices[0]);

		m_StagingBuffer = std::make_shared<VulkanBuffer>(context, BufferType::Staging, dataSize);
		m_StagingBuffer->SetMemory(dataSize, vertices.data());

		m_VertexBuffer = std::make_shared<VulkanBuffer>(context, BufferType::Vertex, dataSize);

		// Copy the staging buffer into the vertex buffer
		VulkanCommandBuffer* commandBuffer = m_Context->GetCommandPool()->AllocateBuffer();
		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToBuffer(m_StagingBuffer.get(), m_VertexBuffer.get(), (uint32_t)dataSize);
		commandBuffer->EndCommands();

		m_Context->SubmitCommandBuffer(commandBuffer);
		m_Context->GetCommandPool()->ReleaseBuffer(commandBuffer);
	}
	void VulkanVertexBuffer::Destroy()
	{
		m_StagingBuffer->Destroy();
		m_StagingBuffer.reset();

		m_VertexBuffer->Destroy();
		m_VertexBuffer.reset();
	}

	VulkanBuffer* VulkanVertexBuffer::GetVertexBuffer()
	{
		return m_VertexBuffer.get();
	}
	const VkBuffer VulkanVertexBuffer::GetVertexBufferVK()
	{
		return m_VertexBuffer->buffer;
	}
	const VkBuffer* VulkanVertexBuffer::GetVertexBufferVKRef()
	{
		return &(m_VertexBuffer->buffer);
	}
}