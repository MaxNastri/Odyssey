#include "RenderGraph.h"

namespace Odyssey
{
	void RenderGraph::Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		RenderGraphNode* node = m_RootNode;

		while (node)
		{
			node->Setup(context, renderingData, commandBufferHandle);
			node = node->GetNext();
		}
	}

	void RenderGraph::Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		RenderGraphNode* node = m_RootNode;

		while (node)
		{
			node->Execute(context, renderingData, commandBufferHandle);
			node = node->GetNext();
		}
	}
}