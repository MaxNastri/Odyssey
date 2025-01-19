#pragma once
#include "DockableWindow.h"
#include "Events.h"
#include "GameObject.h"
#include "Ref.h"
#include "Resource.h"

namespace Odyssey
{
	class RenderObjectsPass;
	class VulkanTextureSampler;

	class GameViewWindow : public DockableWindow
	{
	public:
		GameViewWindow(size_t windowID);
		void Destroy();

	public:
		virtual void Update() override;
		virtual bool Draw() override;
		virtual void OnWindowResize() override;
		virtual void OnWindowClose() override;

	private:
		void OnSceneLoaded(SceneLoadedEvent* event);

	private:
		void CreateRenderTexture();
		void DestroyRenderTexture();

	private:
		uint64_t m_RenderTextureID;
		ResourceID m_RenderTarget;
		ResourceID m_RTSampler;

	private:
		Ref<IEventListener> m_SceneLoadListener = nullptr;
	private:
		Ref<RenderObjectsPass> m_GameViewPass;
		GameObject m_MainCamera;
	};
}