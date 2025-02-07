#pragma once
#include "DockableWindow.h"
#include "PropertyDrawers.h"

namespace Odyssey
{
	enum class SwizzleChannel : uint8_t
	{
		Red = 0,
		Green = 1,
		Blue = 2,
		Alpha = 3,
	};

	class TextureGenerationWindow : public DockableWindow
	{
	public:
		TextureGenerationWindow(size_t windowID);
		void Destroy();

	public:
		virtual void Update() override;
		virtual bool Draw() override;
		virtual void OnWindowClose() override;

	private:
		void AddSwizzleDrawer();

	private:
		struct SwizzleTextureDrawer
		{
			AssetFieldDrawer SrcTextureDrawer;
			EnumDrawer<SwizzleChannel> SrcChannelDrawer;
			EnumDrawer<SwizzleChannel> DstChannelDrawer;
			BoolDrawer InvertDrawer;
		};

		std::vector<SwizzleTextureDrawer> m_Drawers;
		StringDrawer m_DstPathDrawer;
	};
}