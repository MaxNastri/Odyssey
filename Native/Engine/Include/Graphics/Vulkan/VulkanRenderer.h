#pragma once
#include <vulkan/vulkan.h>
#include "VulkanDevice.h"
#include "VulkanDescriptorPool.h"
#include "VulkanImgui.h"
#include "VulkanSwapchain.h"
#include "VulkanFrame.h"
#include "VulkanCommandPool.h"
#include "VulkanShader.h"
#include "ResourceManager.h"
#include "Drawcall.h"
#include "RenderGraph.h"
#include "RenderScene.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanCommandBuffer;
	class VulkanContext;
	class VulkanDescriptorSet;
	class VulkanFrame;
	class VulkanIndexBuffer;
	class VulkanTexture;
	class VulkanVertexBuffer;
	class VulkanWindow;

	struct UBOMatrices
	{
		glm::mat4x4 world;
		glm::mat4x4 proj;
		glm::mat4x4 inverseView;
	};

	class VulkanRenderer
	{
	public:
		VulkanRenderer();
		void Destroy();

	public:
		bool Update();
		bool Render();
		bool Present();

	private:
		void BuildRenderGraph();
		bool BeginFrame(VulkanFrame*& currentFrame);
		void RenderFrame();
		void RebuildSwapchain();

	private:
		VulkanImgui::InitInfo CreateImguiInitInfo();
		void SetupFrameData();

	private: // Vulkan objects
		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<VulkanWindow> window;
		std::unique_ptr<VulkanDescriptorPool> descriptorPool;

	private: // Commands
		std::vector<ResourceHandle<VulkanCommandPool>> commandPools;
		std::vector<ResourceHandle<VulkanCommandBuffer>> commandBuffers;

	private: // UBO
		std::shared_ptr<VulkanDescriptorSet> uboDescriptor;
		std::vector<ResourceHandle<VulkanBuffer>> uboBuffers;
		std::vector<UBOMatrices> uboData;

	private: // Draws
		RenderGraph m_RenderGraph;
		bool m_RenderGraphCreated = false;
		std::shared_ptr<PerFrameRenderingData> renderingData;
		std::vector<Drawcall> m_DrawCalls;

	private: // Render texture stuff
		ResourceHandle<VulkanTexture> renderTexture;
		VkDescriptorSet rtSet;

	private: // IMGUI
		std::shared_ptr<VulkanImgui> imgui;

	private: // Swapchain
		std::unique_ptr<VulkanSwapchain> swapchain;
		bool rebuildSwapchain = false;

	private: // Frame data
		std::vector<VulkanFrame> frames;
		uint32_t frameIndex = 0;
	};
}