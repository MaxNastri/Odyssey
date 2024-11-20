#pragma once
#include "DockableWindow.h"
#include "Resource.h"
#include "Events.h"
#include "GameObject.h"

namespace Odyssey
{
	class OpaquePass;
	class VulkanRenderTexture;
	class VulkanTextureSampler;

	class GameViewWindow : public DockableWindow
	{
	public:
		GameViewWindow(size_t windowID);
		void Destroy();

	public:
		virtual bool Draw() override;
		virtual void OnWindowResize() override;
		virtual void OnWindowClose() override;

	private:
		void OnSceneLoaded(SceneLoadedEvent* event);

	public:
		std::shared_ptr<OpaquePass> GetRenderPass() { return m_GameViewPass; }

	private:
		void CreateRenderTexture();
		void DestroyRenderTexture();

	private:
		uint64_t m_RenderTextureID;
		ResourceID m_ColorRT;
		ResourceID m_DepthRT;
		ResourceID m_RTSampler;
		
	private:
		std::shared_ptr<IEventListener> m_SceneLoadListener = nullptr;
	private:
		std::shared_ptr<OpaquePass> m_GameViewPass;
		GameObject m_MainCamera;
	};
}