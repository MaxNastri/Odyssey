#include "Cubemap.h"
#include "AssetManager.h"
#include "SourceTexture.h"
#include "ResourceManager.h"
#include "VulkanTexture.h"
namespace Odyssey
{
	Cubemap::Cubemap(const Path& assetPath, TextureImportSettings& settings)
		: Asset(assetPath)
	{
		std::shared_ptr<SourceTexture> xNegFace = AssetManager::LoadSourceTexture(settings.GetCubemapFace(CubemapFace::XNeg));
		std::shared_ptr<SourceTexture> xPosFace = AssetManager::LoadSourceTexture(settings.GetCubemapFace(CubemapFace::XPos));
		std::shared_ptr<SourceTexture> yNegFace = AssetManager::LoadSourceTexture(settings.GetCubemapFace(CubemapFace::YNeg));
		std::shared_ptr<SourceTexture> yPosFace = AssetManager::LoadSourceTexture(settings.GetCubemapFace(CubemapFace::YPos));
		std::shared_ptr<SourceTexture> zNegFace = AssetManager::LoadSourceTexture(settings.GetCubemapFace(CubemapFace::ZNeg));
		std::shared_ptr<SourceTexture> zPosFace = AssetManager::LoadSourceTexture(settings.GetCubemapFace(CubemapFace::ZPos));

		BinaryBuffer& xNegBuffer = xNegFace->GetPixelBuffer();
		BinaryBuffer& xPosBuffer = xPosFace->GetPixelBuffer();
		BinaryBuffer& yNegBuffer = yNegFace->GetPixelBuffer();
		BinaryBuffer& yPosBuffer = yPosFace->GetPixelBuffer();
		BinaryBuffer& zNegBuffer = zNegFace->GetPixelBuffer();
		BinaryBuffer& zPosBuffer = zPosFace->GetPixelBuffer();

		BinaryBuffer combinedBuffer;
		combinedBuffer.AppendData(zPosBuffer.GetData());
		combinedBuffer.AppendData(zNegBuffer.GetData());
		combinedBuffer.AppendData(yPosBuffer.GetData());
		combinedBuffer.AppendData(yNegBuffer.GetData());
		combinedBuffer.AppendData(xPosBuffer.GetData());
		combinedBuffer.AppendData(xNegBuffer.GetData());

		m_TextureDescription.ImageType = ImageType::Cubemap;
		m_TextureDescription.Width = zPosFace->GetWidth();
		m_TextureDescription.Height = zPosFace->GetHeight();
		m_TextureDescription.Channels = zPosFace->GetChannels();
		m_TextureDescription.ArrayDepth = 6;

		m_PixelBufferGUID = AssetManager::CreateBinaryAsset(combinedBuffer);
		m_Texture = ResourceManager::Allocate<VulkanTexture>(m_TextureDescription, combinedBuffer);
	}

	void Cubemap::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Cubemap::Load()
	{
		LoadFromDisk(m_AssetPath);
	}

	void Cubemap::SaveToDisk(const Path& assetPath)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the asset metadata first
		SerializeMetadata(serializer);

		root.WriteData("Image Type", (uint32_t)m_TextureDescription.ImageType);
		root.WriteData("Width", m_TextureDescription.Width);
		root.WriteData("Height", m_TextureDescription.Height);
		root.WriteData("Array Depth", m_TextureDescription.ArrayDepth);
		root.WriteData("Channels", m_TextureDescription.Channels);
		root.WriteData("m_PixelBufferGUID", m_PixelBufferGUID.CRef());

		serializer.WriteToDisk(assetPath);
	}

	void Cubemap::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			uint32_t imageType = 0;
			root.ReadData("Image Type", imageType);
			root.ReadData("Width", m_TextureDescription.Width);
			root.ReadData("Height", m_TextureDescription.Height);
			root.ReadData("Array Depth", m_TextureDescription.ArrayDepth);
			root.ReadData("Channels", m_TextureDescription.Channels);
			root.ReadData("m_PixelBufferGUID", m_PixelBufferGUID.Ref());

			m_TextureDescription.ImageType = (ImageType)imageType;
			m_TextureDescription.Format = TextureFormat::R8G8B8A8_UNORM;

			BinaryBuffer pixelBuffer = AssetManager::LoadBinaryAsset(m_PixelBufferGUID);
			m_Texture = ResourceManager::Allocate<VulkanTexture>(m_TextureDescription, pixelBuffer);
		}
	}
}