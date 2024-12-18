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

		if (m_ColorTexture)
			root.WriteData("m_Texture", m_ColorTexture->GetGUID().CRef());

		if (m_NormalTexture)
			root.WriteData("Normal Texture", m_NormalTexture->GetGUID().CRef());

		root.WriteData("Emissive Color", m_EmissiveColor);
		root.WriteData("Emissive Power", m_EmissivePower);
		root.WriteData("Alpha Clip", m_AlphaClip);

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
			GUID colorTextureGUID;
			GUID normalTextureGUID;

			root.ReadData("m_Shader", shaderGUID.Ref());
			root.ReadData("m_Texture", colorTextureGUID.Ref());
			root.ReadData("Normal Texture", normalTextureGUID.Ref());
			root.ReadData("Emissive Color", m_EmissiveColor);
			root.ReadData("Emissive Power", m_EmissivePower);
			root.ReadData("Alpha Clip", m_AlphaClip);

			if (shaderGUID)
				m_Shader = AssetManager::LoadAsset<Shader>(shaderGUID);

			if (colorTextureGUID)
				m_ColorTexture = AssetManager::LoadAsset<Texture2D>(colorTextureGUID);

			if (normalTextureGUID)
				m_NormalTexture = AssetManager::LoadAsset<Texture2D>(normalTextureGUID);
		}
	}
}