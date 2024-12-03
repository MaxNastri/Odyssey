#pragma once
#include "Ref.h"
#include "Texture2D.h"
#include "GUID.h"

namespace Odyssey
{
	class ThumbnailManager
	{
	public:
		ThumbnailManager() = default;

	public:
		static uint64_t LoadThumbnail(const Path& path);
		static uint64_t LoadThumbnail(const GUID& guid);
		static void UnloadTexture(uint64_t handle);

	public:
		static Ref<Texture2D> GetTexture(uint64_t handle);

	private:
		struct TextureData
		{
			Ref<Texture2D> Texture;
			uint64_t ImGuiHandle;
		};

		inline static std::map<GUID, TextureData> m_TextureDatas;
	};
}