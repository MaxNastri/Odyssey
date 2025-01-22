#pragma once
#include "VulkanGlobals.h"
#include "Resource.h"

// FWD Declarations
struct GLFWwindow;

VK_FWD_DECLARE(VkDescriptorPool)

namespace Odyssey
{
	class VulkanDevice;
	class VulkanContext;
	class GUIElement;
	class VulkanTextureSampler;
	class VulkanCommandBuffer;

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
		void Update();
		void Render(ResourceID commandBufferID);
		void PostRender();
		void SetDrawGUIListener(std::function<void(void)> listener) { m_DrawGUIListener = listener; }

	public:
		uint64_t AddTexture(ResourceID textureID);
		uint64_t AddRenderTexture(ResourceID renderTextureID, ResourceID samplerID);
		void RemoveTexture(uint64_t id);
		void SetFont(Path fontFile, float fontSize);
		void SetMouseInputEnabled(bool enabled) { m_MouseInputEnabled = enabled; }

	private:
		void CreateDescriptorPool();
		void UploadFont();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		std::map<int64_t, VkDescriptorSet> m_RenderTextures;
		VkDescriptorPool descriptorPool;
		std::function<void(void)> m_DrawGUIListener;
		bool m_MouseInputEnabled = false;

	private:
		std::vector<std::function<void()>> m_PendingDestroys;
	};
}