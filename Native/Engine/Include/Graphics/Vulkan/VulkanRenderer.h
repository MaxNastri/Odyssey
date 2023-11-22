#pragma once
#include <vulkan/vulkan.h>
#include "VulkanDevice.h"
#include "VulkanDescriptorPool.h"
#include "VulkanImgui.h"
#include "VulkanSwapchain.h"
#include "VulkanFrame.h"
#include "VulkanCommandPool.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanShader.h"
#include "VulkanVertex.h"
#include "ResourceManager.h"

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

	struct DrawCall
	{
	public:
		ResourceHandle<VulkanVertexBuffer> VertexBuffer;
		ResourceHandle<VulkanIndexBuffer> IndexBuffer;
		uint32_t IndexCount;
	};

	struct RenderObject
	{
	public:
		RenderObject(std::vector<VulkanVertex> vertices, std::vector<uint32_t> indices)
		{
			Vertices = vertices;
			Indices = indices;
		}

		std::vector<VulkanVertex> Vertices;
		std::vector<uint32_t> Indices;
	};

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
		bool BeginFrame(VulkanFrame*& currentFrame);
		void RenderFrame();
		void RebuildSwapchain();
		void InitDrawCalls();

	private:
		VulkanImgui::InitInfo CreateImguiInitInfo();
		void SetupFrameData();
		VkRenderingInfo GetRenderingInfo(VulkanFrame* frame);

	private: // Vulkan objects
		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<VulkanWindow> window;
		std::unique_ptr<VulkanDescriptorPool> descriptorPool;

	private: // Commands
		std::vector<std::unique_ptr<VulkanCommandPool>> commandPool;
		std::vector<VulkanCommandBuffer*> commandBuffers;

	private: // Pipeline
		std::unique_ptr<VulkanGraphicsPipeline> graphicsPipeline;
		ResourceHandle<VulkanShader> fragmentShader;
		ResourceHandle<VulkanShader> vertexShader;

	private: // UBO
		std::shared_ptr<VulkanDescriptorSet> uboDescriptor;
		std::vector<std::shared_ptr<VulkanBuffer>> uboBuffers;
		std::vector<UBOMatrices> uboData;

	private: // Draws
		std::vector<DrawCall> m_DrawCalls;
		std::vector<RenderObject> m_RenderObjects;
		std::vector<ResourceHandle<VulkanVertexBuffer>> m_VertexBuffers;
		std::vector<ResourceHandle<VulkanIndexBuffer>> m_IndexBuffers;

	private: // Render texture stuff
		ResourceHandle<VulkanTexture> renderTexture;
		VkDescriptorSet rtSet;

	private: // IMGUI
		std::unique_ptr<VulkanImgui> imgui;

	private: // Swapchain
		std::unique_ptr<VulkanSwapchain> swapchain;
		bool rebuildSwapchain = false;

	private: // Frame data
		std::vector<VulkanFrame> frames;
		uint32_t frameIndex = 0;
	};
}