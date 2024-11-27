#include "ThumbnailManager.h"
#include "AssetManager.h"
#include "Renderer.h"

namespace Odyssey
{
	uint64_t ThumbnailManager::LoadTexture(const Path& path)
	{
		return LoadTexture(AssetManager::PathToGUID(path));
	}

	uint64_t ThumbnailManager::LoadTexture(const GUID& guid)
	{
		if (guid)
		{
			if (m_TextureDatas.contains(guid))
				return m_TextureDatas[guid].ImGuiHandle;

			auto& textureData = m_TextureDatas[guid];
			textureData.Texture = AssetManager::LoadAsset<Texture2D>(guid);
			textureData.ImGuiHandle = Renderer::AddImguiTexture(textureData.Texture);
			return textureData.ImGuiHandle;
		}

		return 0;
	}

	void ThumbnailManager::UnloadTexture(uint64_t handle)
	{
		for (auto& [guid, textureData] : m_TextureDatas)
		{
			if (textureData.ImGuiHandle == handle)
			{
				Renderer::DestroyImguiTexture(handle);
				m_TextureDatas.erase(guid);
			}
		}
	}
}