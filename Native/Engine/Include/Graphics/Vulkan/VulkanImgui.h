#pragma once
#include "VulkanGlobals.h"
#include "ResourceHandle.h"

// FWD Declarations
struct GLFWwindow;

VK_FWD_DECLARE(VkDescriptorPool)

namespace Odyssey
{
	class VulkanDevice;
	class VulkanContext;
	class GUIElement;
	class VulkanTexture;


	class VulkanImgui
	{
	public:
		struct InitInfo
		{
			GLFWwindow* window;
			VkInstance instance;
			VkPhysicalDevice physicalDevice;
			VkDevice logicalDevice;
			uint32_t queueIndex;
			VkQueue queue;
			VkDescriptorPool descriptorPool;
			VkRenderPass renderPass;
			uint32_t minImageCount;
			uint32_t imageCount;
			VkFormat colorFormat;
		};

	public:
		VulkanImgui(std::shared_ptr<VulkanContext> context, const InitInfo& initInfo);
		void SubmitDraws();
		void Render(VkCommandBuffer commandBuffer);
		void PostRender();

	public:
		uint64_t AddTexture(ResourceHandle<VulkanTexture> handle);
		void RemoveTexture(uint64_t id);

	private:
		void CreateDescriptorPool();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		std::map<int64_t, VkDescriptorSet> m_RenderTextures;
		VkDescriptorPool descriptorPool;
	};
}