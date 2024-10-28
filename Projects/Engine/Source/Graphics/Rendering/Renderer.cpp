#include "Renderer.h"
#include "VulkanRenderer.h"
#include "ParticleBatcher.h"
#include "Texture2D.h"

namespace Odyssey
{
	void Renderer::Init(const RendererConfig& config)
	{
		s_Config = config;
		s_RendererAPI = std::make_shared<VulkanRenderer>();

		if (s_Config.EnableIMGUI)
			s_RendererAPI->AddImguiPass();

		ParticleBatcher::Init();
	}

	bool Renderer::Update()
	{
		return s_RendererAPI->Update();
	}

	bool Renderer::Render()
	{
		return s_RendererAPI->Render();
	}

	void Renderer::Destroy()
	{
		s_RendererAPI->Destroy();
	}

	void Renderer::PushRenderPass(std::shared_ptr<RenderPass> renderPass)
	{
		s_RendererAPI->AddRenderPass(renderPass);
	}

	void Renderer::SetDrawGUIListener(std::function<void(void)> listener)
	{
		s_RendererAPI->GetImGui()->SetDrawGUIListener(listener);
	}

	uint64_t Renderer::AddImguiTexture(std::shared_ptr<Texture2D> texture)
	{
		return s_RendererAPI->GetImGui()->AddTexture(texture->GetTexture());
	}

	uint64_t Renderer::AddImguiRenderTexture(ResourceID renderTextureID, ResourceID samplerID)
	{
		return s_RendererAPI->GetImGui()->AddRenderTexture(renderTextureID, samplerID);
	}

	void Renderer::DestroyImguiTexture(uint64_t textureHandle)
	{
		s_RendererAPI->GetImGui()->RemoveTexture(textureHandle);
	}

	std::shared_ptr<VulkanWindow> Renderer::GetWindow()
	{
		return s_RendererAPI->GetWindow();
	}

	void Renderer::RegisterRenderTarget(RenderTargetType rtType, ResourceID colorRT, ResourceID depthRT)
	{
		auto& renderTarget = m_RenderTargets[rtType];
		renderTarget.Type = rtType;
		renderTarget.ColorAttachment = colorRT;
		renderTarget.DepthAttachment = depthRT;
	}
}