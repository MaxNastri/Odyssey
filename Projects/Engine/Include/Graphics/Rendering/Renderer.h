#pragma once
#include "Ref.h"
#include "Resource.h"

namespace Odyssey
{
	class Texture2D;
	class RenderPass;
	class VulkanRenderer;
	class VulkanTextureSampler;
	class VulkanWindow;

	enum class RenderTargetType
	{
		None = 0,
		BackBuffer = 1,
		SceneView = 2,
		GameView = 3,
	};

	struct RendererConfig
	{
	public:
		bool EnableIMGUI = true;
		bool ShadowsEnabled = true;
	};

	class Renderer
	{
	public:
		static void Init(const RendererConfig& config);
		static bool Update();
		static bool Render();
		static void Destroy();

	public:
		static void PushRenderPass(Ref<RenderPass> renderPass);
		static void SetDrawGUIListener(std::function<void(void)> listener);
		static uint64_t AddImguiTexture(Ref<Texture2D> texture);
		static uint64_t AddImguiRenderTexture(ResourceID renderTextureID, ResourceID samplerID);
		static void DestroyImguiTexture(uint64_t imguiHandle);
		static std::shared_ptr<VulkanWindow> GetWindow();

	public:
		static void RegisterRenderTarget(RenderTargetType rtType, ResourceID colorRT, ResourceID depthRT);

	private:
		inline static std::shared_ptr<VulkanRenderer> s_RendererAPI;
		inline static RendererConfig s_Config;
	};
}