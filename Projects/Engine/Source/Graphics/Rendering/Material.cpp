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

		if (m_Shader)
			root.WriteData("m_Shader", m_Shader->GetGUID().CRef());

		if (m_Texture)
			root.WriteData("m_Texture", m_Texture->GetGUID().CRef());

		// Save to disk
		serializer.WriteToDisk(assetPath);
	}

	void Material::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			GUID shaderGUID;
			GUID textureGUID;

			root.ReadData("m_Shader", shaderGUID.Ref());
			root.ReadData("m_Texture", textureGUID.Ref());

			if (shaderGUID)
				m_Shader = AssetManager::LoadShaderByGUID(shaderGUID);

			if (textureGUID)
				m_Texture = AssetManager::LoadTexture2DByGUID(textureGUID);
		}
	}
}