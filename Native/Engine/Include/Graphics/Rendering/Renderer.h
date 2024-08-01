#pragma once
#include "ResourceHandle.h"

namespace Odyssey
{
	class RenderPass;
	class VulkanRenderer;
	class VulkanRenderTexture;
	class VulkanTextureSampler;
	class VulkanWindow;

	struct RendererConfig
	{
	public:
		bool EnableIMGUI = true;
	};

	class Renderer
	{
	public:
		static void Init(const RendererConfig& config);
		static bool Update();
		static bool Render();
		static void Destroy();

	public:
		static void PushRenderPass(std::shared_ptr<RenderPass> renderPass);
		static void SetDrawGUIListener(std::function<void(void)> listener);
		static int64_t AddImguiTexture(ResourceHandle<VulkanRenderTexture> textureHandle, ResourceHandle<VulkanTextureSampler> samplerHandle);
		static std::shared_ptr<VulkanWindow> GetWindow();

	private:
		inline static std::shared_ptr<VulkanRenderer> s_RendererAPI;
		inline static RendererConfig s_Config;
	};
}