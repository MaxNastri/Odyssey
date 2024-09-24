#pragma once
#include "Resource.h"

namespace Odyssey
{
	class RenderPass;
	class VulkanRenderer;
	class VulkanRenderTexture;
	class VulkanTextureSampler;
	class VulkanWindow;

	enum class RenderTargetType
	{
		None = 0,
		BackBuffer = 1,
		SceneView = 2,
		GameView = 3,
	};
	struct RenderTarget
	{
		RenderTargetType Type;
		ResourceID ColorAttachment;
		ResourceID DepthAttachment;
	};

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
		static int64_t AddImguiTexture(ResourceID renderTextureID, ResourceID samplerID);
		static std::shared_ptr<VulkanWindow> GetWindow();

	public:
		static void RegisterRenderTarget(RenderTargetType rtType, ResourceID colorRT, ResourceID depthRT);

	private:

		inline static std::unordered_map<RenderTargetType, RenderTarget> m_RenderTargets;
		inline static std::shared_ptr<VulkanRenderer> s_RendererAPI;
		inline static RendererConfig s_Config;
	};
}