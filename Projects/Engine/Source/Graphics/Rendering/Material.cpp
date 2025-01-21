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

		SerializationNode texturesNode = root.CreateSequenceNode("Property Textures");
		for (auto& [propertyName, texture] : m_Textures)
		{
			SerializationNode textureNode = texturesNode.AppendChild();
			textureNode.SetMap();
			textureNode.WriteData("Property", propertyName);
			textureNode.WriteData("Texture", texture->GetGUID().CRef());
		}

		root.WriteData("Emissive Color", m_EmissiveColor);
		root.WriteData("Emissive Power", m_EmissivePower);
		root.WriteData("Alpha Clip", m_AlphaClip);
		root.WriteData("Render Queue", Enum::ToInt(m_RenderQueue));
		root.WriteData("Blend Mode", Enum::ToString(m_BlendMode));
		root.WriteData("Depth Write", m_DepthWrite);

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
			int32_t renderQueue = 0;
			std::string blendMode;

			root.ReadData("m_Shader", shaderGUID.Ref());

			SerializationNode texturesNode = root.GetNode("Property Textures");
			for (size_t i = 0; i < texturesNode.ChildCount(); i++)
			{
				SerializationNode textureNode = texturesNode.GetChild(i);
				assert(textureNode.IsMap());
			
				std::string property;
				GUID textureGUID;
				textureNode.ReadData("Property", property);
				textureNode.ReadData("Texture", textureGUID.Ref());
			
				if (!property.empty() && textureGUID)
					m_Textures[property] = AssetManager::LoadAsset<Texture2D>(textureGUID);
			}

			root.ReadData("Emissive Color", m_EmissiveColor);
			root.ReadData("Emissive Power", m_EmissivePower);
			root.ReadData("Alpha Clip", m_AlphaClip);
			root.ReadData("Render Queue", renderQueue);
			root.ReadData("Blend Mode", blendMode);
			root.ReadData("Depth Write", m_DepthWrite);

			if (shaderGUID)
				m_Shader = AssetManager::LoadAsset<Shader>(shaderGUID);

			if (renderQueue > 0)
				m_RenderQueue = Enum::ToEnum<RenderQueue>(renderQueue);

			if (!blendMode.empty())
				m_BlendMode = Enum::ToEnum<BlendMode>(blendMode);
		}
	}

	void Material::SetTexture(std::string propertyName, GUID texture)
	{
		m_Textures[propertyName] = AssetManager::LoadAsset<Texture2D>(texture);
	}
}