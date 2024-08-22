#include "Material.h"
#include "Shader.h"
#include "Texture2D.h"
#include "AssetManager.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	Material::Material(const Path& assetPath)
		: Asset(assetPath)
	{
		LoadFromDisk(assetPath);
	}

	void Material::Save()
	{
		if (!m_AssetPath.empty())
		{
			SaveToDisk(m_AssetPath);
		}
	}

	void Material::Load()
	{
		if (!m_AssetPath.empty())
		{
			LoadFromDisk(m_AssetPath);
		}
	}

	void Material::SaveToDisk(const Path& assetPath)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize metadata first
		SerializeMetadata(serializer);

		if (m_FragmentShader)
			root.WriteData("m_FragmentShader", m_FragmentShader->GetGUID());

		if (m_VertexShader)
			root.WriteData("m_VertexShader", m_VertexShader->GetGUID());

		if (m_Texture)
			root.WriteData("m_Texture", m_Texture->GetGUID());

		// Save to disk
		serializer.WriteToDisk(assetPath);
	}

	void Material::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			std::string fragGUID;
			std::string vertGUID;
			std::string textureGUID;

			root.ReadData("m_FragmentShader", fragGUID);
			root.ReadData("m_VertexShader", vertGUID);
			root.ReadData("m_Texture", textureGUID);

			if (!fragGUID.empty())
				m_FragmentShader = AssetManager::LoadShaderByGUID(fragGUID);
			if (!vertGUID.empty())
				m_VertexShader = AssetManager::LoadShaderByGUID(vertGUID);
			if (!textureGUID.empty())
				m_Texture = AssetManager::LoadTexture2DByGUID(textureGUID);
		}
	}
}