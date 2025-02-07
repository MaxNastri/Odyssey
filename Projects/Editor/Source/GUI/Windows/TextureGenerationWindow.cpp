#include "TextureGenerationWindow.h"
#include "GUIManager.h"
#include "AssetManager.h"
#include "Project.h"
#include "stb_image_write.h"
namespace Odyssey
{
	TextureGenerationWindow::TextureGenerationWindow(size_t windowID)
		: DockableWindow("Texture Generation Window", windowID,
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		AddSwizzleDrawer();
		m_DstPathDrawer = StringDrawer("Destination Path", "");
	}

	void TextureGenerationWindow::Destroy()
	{

	}

	void TextureGenerationWindow::Update()
	{

	}


	void FillChannel(BinaryBuffer& dstPixelBuffer, SwizzleChannel swizzleChannel, uint8_t color)
	{
		std::vector<uint8_t> dstPixels = dstPixelBuffer.GetData();
		uint8_t channel = (uint8_t)swizzleChannel;

		for (size_t i = 0; i < dstPixels.size(); i += 4)
		{
			dstPixels[i + channel] = color;
		}

		dstPixelBuffer.WriteData(dstPixels);
	}

	void Swizzle(BinaryBuffer& srcPixelBuffer, SwizzleChannel srcSwizzleChannel, uint32_t srcWidth, uint32_t srcHeight, BinaryBuffer& dstPixelBuffer, SwizzleChannel dstSwizzleChannel, bool invert)
	{
		std::vector<uint8_t> srcPixels = srcPixelBuffer.GetData();
		std::vector<uint8_t> dstPixels = dstPixelBuffer.GetData();

		uint8_t srcChannel = (uint8_t)srcSwizzleChannel;
		uint8_t dstChannel = (uint8_t)dstSwizzleChannel;

		for (size_t i = 0; i < srcPixels.size(); i += 4)
		{
			uint8_t color = srcPixels[i + srcChannel];
			dstPixels[i + dstChannel] = invert ? 255 - color : color;
		}

		dstPixelBuffer.WriteData(dstPixels);
	}

	bool TextureGenerationWindow::Draw()
	{
		bool modified = false;
		for (SwizzleTextureDrawer& drawer : m_Drawers)
		{
			modified |= drawer.SrcTextureDrawer.Draw();
			modified |= drawer.SrcChannelDrawer.Draw();
			modified |= drawer.DstChannelDrawer.Draw();
			modified |= drawer.InvertDrawer.Draw();
		}

		modified |= m_DstPathDrawer.Draw();

		if (ImGui::Button("Add Swizzle"))
			AddSwizzleDrawer();

		ImGui::SameLine();

		if (ImGui::Button("Remove Swizzle"))
			m_Drawers.erase(m_Drawers.end());

		if (ImGui::Button("Generate") && m_Drawers.size() > 0)
		{
			// Get the first source texture
			Ref<SourceTexture> srcTexture = AssetManager::LoadSourceAsset<SourceTexture>(m_Drawers[0].SrcTextureDrawer.GetGUID());
			BinaryBuffer srcBuffer = srcTexture->GetPixelBuffer();

			// Create a binary buffer for the swizzle pixel data
			BinaryBuffer dstBuffer = BinaryBuffer(nullptr, srcBuffer.GetSize());
			FillChannel(dstBuffer, SwizzleChannel::Alpha, 255);

			// Swizzle the first source texture
			SwizzleTextureDrawer& drawer = m_Drawers[0];
			Swizzle(srcBuffer, drawer.SrcChannelDrawer.GetValue(), srcTexture->GetWidth(), srcTexture->GetHeight(),
				dstBuffer, drawer.DstChannelDrawer.GetValue(), drawer.InvertDrawer.GetValue());

			// Swizzle the rest
			for (size_t i = 1; i < m_Drawers.size(); i++)
			{
				drawer = m_Drawers[i];
				srcTexture = AssetManager::LoadSourceAsset<SourceTexture>(drawer.SrcTextureDrawer.GetGUID());
				Swizzle(srcBuffer, drawer.SrcChannelDrawer.GetValue(), srcTexture->GetWidth(), srcTexture->GetHeight(),
					dstBuffer, drawer.DstChannelDrawer.GetValue(), drawer.InvertDrawer.GetValue());
			}

			// Flush to disk
			Path dstPath = Project::GetActiveAssetsDirectory() / m_DstPathDrawer.GetValue();
			stbi_write_png(dstPath.string().c_str(), srcTexture->GetWidth(), srcTexture->GetHeight(), 4, dstBuffer.GetData().data(), 0);
		}

		return modified;
	}

	void TextureGenerationWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}

	void TextureGenerationWindow::AddSwizzleDrawer()
	{
		if (m_Drawers.size() < 4)
		{
			SwizzleTextureDrawer& drawer = m_Drawers.emplace_back();
			SwizzleChannel channel = (SwizzleChannel)(m_Drawers.size() - 1);
			drawer.SrcTextureDrawer = AssetFieldDrawer("Source Texture", GUID::Empty(), SourceTexture::Type);
			drawer.SrcChannelDrawer = EnumDrawer<SwizzleChannel>("Source Channel", channel);
			drawer.DstChannelDrawer = EnumDrawer<SwizzleChannel>("Destination Channel", channel);
			drawer.InvertDrawer = BoolDrawer("Invert", false);
		}
	}
}