#pragma once
#include "DockableWindow.h"
#include "AssetFieldDrawer.h"
#include "StringDrawer.h"
#include "GUID.h"

namespace Odyssey
{
	class CubemapCreationWindow : public DockableWindow
	{
	public:
		CubemapCreationWindow();

	public:
		virtual void Draw() override;

	private:
		GUID m_XPosFaceGUID;
		GUID m_XNegFaceGUID;
		GUID m_YPosFaceGUID;
		GUID m_YNegFaceGUID;
		GUID m_ZPosFaceGUID;
		GUID m_ZNegFaceGUID;
		AssetFieldDrawer m_XPosFaceDrawer;
		AssetFieldDrawer m_XNegFaceDrawer;
		AssetFieldDrawer m_YPosFaceDrawer;
		AssetFieldDrawer m_YNegFaceDrawer;
		AssetFieldDrawer m_ZPosFaceDrawer;
		AssetFieldDrawer m_ZNegFaceDrawer;
		StringDrawer m_AssetPathDrawer;

	};
}