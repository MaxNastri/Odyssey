#include "CubemapCreationWindow.h"
#include "imgui.h"
#include "TextureImportSettings.h"
#include "AssetManager.h"
#include "Project.h"

namespace Odyssey
{
	CubemapCreationWindow::CubemapCreationWindow()
		: DockableWindow("Cubemap Creation Window",
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		m_XPosFaceDrawer = AssetFieldDrawer("X+ Face", 0, "SourceTexture", nullptr);
		m_XNegFaceDrawer = AssetFieldDrawer("X- Face", 0, "SourceTexture", nullptr);
		m_YPosFaceDrawer = AssetFieldDrawer("Y- Face", 0, "SourceTexture", nullptr);
		m_YNegFaceDrawer = AssetFieldDrawer("Y- Face", 0, "SourceTexture", nullptr);
		m_ZPosFaceDrawer = AssetFieldDrawer("Z+ Face", 0, "SourceTexture", nullptr);
		m_ZNegFaceDrawer = AssetFieldDrawer("Z- Face", 0, "SourceTexture", nullptr);
		m_AssetPathDrawer = StringDrawer("Asset Path", "", nullptr, false);
	}

	void CubemapCreationWindow::Draw()
	{
		if (!Begin())
			return;

		m_XPosFaceDrawer.Draw();
		m_XNegFaceDrawer.Draw();
		m_YPosFaceDrawer.Draw();
		m_YNegFaceDrawer.Draw();
		m_ZPosFaceDrawer.Draw();
		m_ZNegFaceDrawer.Draw();
		m_AssetPathDrawer.Draw();

		if (!m_AssetPathDrawer.GetValue().empty() && ImGui::Button("Create Cubemap"))
		{
			TextureImportSettings settings;
			settings.SetTextureType(TextureType::Cubemap);
			settings.SetCubemapFace(CubemapFace::XNeg, m_XNegFaceDrawer.GetGUID());
			settings.SetCubemapFace(CubemapFace::XPos, m_XPosFaceDrawer.GetGUID());
			settings.SetCubemapFace(CubemapFace::YNeg, m_YNegFaceDrawer.GetGUID());
			settings.SetCubemapFace(CubemapFace::YPos, m_YPosFaceDrawer.GetGUID());
			settings.SetCubemapFace(CubemapFace::ZNeg, m_ZNegFaceDrawer.GetGUID());
			settings.SetCubemapFace(CubemapFace::ZPos, m_ZPosFaceDrawer.GetGUID());

			Path assetPath = Project::GetActiveAssetsDirectory() / m_AssetPathDrawer.GetValue();
			AssetManager::CreateCubemap(assetPath, settings);
		}
		End();
	}
}