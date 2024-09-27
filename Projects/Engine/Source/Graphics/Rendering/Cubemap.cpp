#include "Cubemap.h"
#include "AssetManager.h"
#include "SourceTexture.h"
#include "ResourceManager.h"
#include "VulkanTexture.h"
#include "CubemapConverter.hpp"

namespace Odyssey
{
	Cubemap::Cubemap(const Path& assetPath)
		: Asset(assetPath)
	{
		Load();
	}

	Cubemap::Cubemap(const Path& assetPath, std::shared_ptr<SourceTexture> source)
		: Asset(assetPath)
	{
		SetSourceAsset(source->GetGUID());

		// Convert the source texture into 6 faces
		HdriToCubemap<unsigned char> hdriToCube_ldr(source->GetPath().string(), 1024, true);

		size_t resolution = hdriToCube_ldr.getCubemapResolution();
		size_t channels = hdriToCube_ldr.getNumChannels();
		size_t bufferSize = resolution * resolution * channels;

		BinaryBuffer rightBuffer(hdriToCube_ldr.getRight(), bufferSize);
		BinaryBuffer leftBuffer(hdriToCube_ldr.getLeft(), bufferSize);
		BinaryBuffer upBuffer(hdriToCube_ldr.getUp(), bufferSize);
		BinaryBuffer downBuffer(hdriToCube_ldr.getDown(), bufferSize);
		BinaryBuffer frontBuffer(hdriToCube_ldr.getFront(), bufferSize);
		BinaryBuffer backBuffer(hdriToCube_ldr.getBack(), bufferSize);

		BinaryBuffer combinedBuffer;
		combinedBuffer.AppendData(rightBuffer.GetData());
		combinedBuffer.AppendData(leftBuffer.GetData());
		combinedBuffer.AppendData(upBuffer.GetData());
		combinedBuffer.AppendData(downBuffer.GetData());
		combinedBuffer.AppendData(frontBuffer.GetData());
		combinedBuffer.AppendData(backBuffer.GetData());

		m_TextureDescription.ImageType = ImageType::Cubemap;
		m_TextureDescription.Width = resolution;
		m_TextureDescription.Height = resolution;
		m_TextureDescription.Channels = channels;
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