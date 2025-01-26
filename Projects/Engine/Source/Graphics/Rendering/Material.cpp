#include "Material.h"
#include "Shader.h"
#include "Texture2D.h"
#include "AssetManager.h"
#include "AssetSerializer.h"
#include "VulkanGraphicsPipeline.h"
#include "ResourceManager.h"

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
			{
				m_Shader = AssetManager::LoadAsset<Shader>(shaderGUID);
				auto onShaderModified = [this]() { OnShaderModified(); };
				m_Shader->AddOnModifiedListener(onShaderModified);
			}

			if (renderQueue > 0)
				m_RenderQueue = Enum::ToEnum<RenderQueue>(renderQueue);

			if (!blendMode.empty())
				m_BlendMode = Enum::ToEnum<BlendMode>(blendMode);
		}

		if (m_Shader)
			CreatePipeline();
	}

	void Material::CreatePipeline()
	{
		if (m_GraphicsPipeline.IsValid())
			ResourceManager::Destroy(m_GraphicsPipeline);

		// Create the pipeline
		VulkanPipelineInfo info;
		info.Shaders = m_Shader->GetResourceMap();
		info.DescriptorLayout = m_Shader->GetDescriptorLayout();
		info.CullMode = CullMode::Back;
		info.SetBlendMode = m_BlendMode;
		info.WriteDepth = m_DepthWrite;
		info.AttributeDescriptions = m_Shader->GetVertexAttributes();
		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
	}

	void Material::OnShaderModified()
	{
		m_RemakePipeline = true;
	}

	ResourceID Material::GetPipeline()
	{
		if (m_RemakePipeline)
		{
			CreatePipeline();
			m_RemakePipeline = false;
		}

		return m_GraphicsPipeline;
	}

	void Material::SetShader(Ref<Shader> shader)
	{
		m_Shader = shader;
		m_RemakePipeline = true;
	}

	void Material::SetTexture(std::string propertyName, GUID texture)
	{
		m_Textures[propertyName] = AssetManager::LoadAsset<Texture2D>(texture);
	}

	void Material::SetBlendMode(BlendMode blendMode)
	{
		m_BlendMode = blendMode;
		m_RemakePipeline = true;
	}

	void Material::SetDepthWrite(bool write)
	{
		m_DepthWrite = write;
		m_RemakePipeline = true;
	}
}