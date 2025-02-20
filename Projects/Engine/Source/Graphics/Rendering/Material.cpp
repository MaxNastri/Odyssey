#include "Material.h"
#include "Shader.h"
#include "Texture2D.h"
#include "AssetManager.h"
#include "AssetSerializer.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "ResourceManager.h"
#include "Enum.h"

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

		root.WriteData("Render Queue", Enum::ToString(m_RenderQueue));
		root.WriteData("Blend Mode", Enum::ToString(m_BlendMode));
		root.WriteData("Depth Write", m_DepthWrite);

		SerializationNode texturesNode = root.CreateSequenceNode("Property Textures");
		for (auto& [propertyName, texture] : m_Textures)
		{
			SerializationNode textureNode = texturesNode.AppendChild();
			textureNode.SetMap();
			textureNode.WriteData("Property", propertyName);
			textureNode.WriteData("Texture", texture->GetGUID().CRef());
		}

		SerializationNode propertiesNode = root.CreateSequenceNode("Properties");
		for (MaterialProperty& materialProperty : m_MaterialData.Properties)
		{
			SerializationNode propertyNode = propertiesNode.AppendChild();
			propertyNode.SetMap();
			propertyNode.WriteData("Name", materialProperty.Name);
			propertyNode.WriteData("Type", Enum::ToString(materialProperty.Type));

			switch (materialProperty.Type)
			{
				case PropertyType::Float:
					propertyNode.WriteData("Value", m_MaterialData.GetValue<float>(materialProperty.Name));
					break;
				case PropertyType::Float2:
					propertyNode.WriteData("Value", m_MaterialData.GetValue<float2>(materialProperty.Name));
					break;
				case PropertyType::Float3:
					propertyNode.WriteData("Value", m_MaterialData.GetValue<float3>(materialProperty.Name));
					break;
				case PropertyType::Float4:
					propertyNode.WriteData("Value", m_MaterialData.GetValue<float4>(materialProperty.Name));
					break;
				case PropertyType::Bool:
					propertyNode.WriteData("Value", m_MaterialData.GetValue<bool>(materialProperty.Name));
					break;
				case PropertyType::Int32:
					propertyNode.WriteData("Value", m_MaterialData.GetValue<int32_t>(materialProperty.Name));
					break;
				default:
					break;
			}
		}

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
			std::string renderQueue;
			std::string blendMode;

			root.ReadData("m_Shader", shaderGUID.Ref());

			if (shaderGUID)
				SetShader(AssetManager::LoadAsset<Shader>(shaderGUID));

			root.ReadData("Render Queue", renderQueue);
			root.ReadData("Blend Mode", blendMode);
			root.ReadData("Depth Write", m_DepthWrite);

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

			SerializationNode propertiesNode;
			if (root.TryGetNode("Properties", propertiesNode))
			{
				for (size_t i = 0; i < propertiesNode.ChildCount(); i++)
				{
					SerializationNode propertyNode = propertiesNode.GetChild(i);
					assert(propertyNode.IsMap());

					std::string name;
					std::string typeName;
					PropertyType propertyType = PropertyType::Unknown;

					propertyNode.ReadData("Name", name);
					propertyNode.ReadData("Type", typeName);

					if (!typeName.empty())
						propertyType = Enum::ToEnum<PropertyType>(typeName);

					if (m_MaterialData.PropertyMap.contains(name))
					{
						switch (propertyType)
						{
							case Odyssey::PropertyType::Float:
							{
								float serializedValue;
								propertyNode.ReadData("Value", serializedValue);
								m_MaterialData.SetValue(name, &serializedValue);
								break;
							}
							case Odyssey::PropertyType::Float2:
							{
								float2 serializedValue;
								propertyNode.ReadData("Value", serializedValue);
								m_MaterialData.SetValue(name, &serializedValue);
								break;
							}
							case Odyssey::PropertyType::Float3:
							{
								float3 serializedValue;
								propertyNode.ReadData("Value", serializedValue);
								m_MaterialData.SetValue(name, &serializedValue);
								break;
							}
							case Odyssey::PropertyType::Float4:
							{
								float4 serializedValue;
								propertyNode.ReadData("Value", serializedValue);
								m_MaterialData.SetValue(name, &serializedValue);
								break;
							}
							case Odyssey::PropertyType::Bool:
							{
								bool serializedValue;
								propertyNode.ReadData("Value", serializedValue);
								m_MaterialData.SetValue(name, &serializedValue);
								break;
							}
							case Odyssey::PropertyType::Int32:
							{
								int32_t serializedValue;
								propertyNode.ReadData("Value", serializedValue);
								m_MaterialData.SetValue(name, &serializedValue);
								break;
							}
						}
					}
				}
			}
			
			if (!renderQueue.empty())
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

	ResourceID Material::GetMaterialBuffer()
	{
		if (m_MaterialBuffer.IsValid() && m_UpdateBuffer)
		{
			Ref<VulkanBuffer> materialBuffer = ResourceManager::GetResource<VulkanBuffer>(m_MaterialBuffer);
			materialBuffer->CopyData(m_MaterialData.Size, m_MaterialData.Buffer.GetData());
		}

		return m_MaterialBuffer;
	}

	void Material::SetShader(Ref<Shader> shader)
	{
		// Clear our previous listener before setting the new shader
		if (m_Shader)
			m_Shader->RemoveOnModifiedListener(m_ListenerID);

		m_Shader = shader;

		// Listen for when the new shader is modified
		m_ListenerID = m_Shader->AddOnModifiedListener([this]() { OnShaderModified(); });

		// Copy the material buffer data from the shader (including defaults)
		m_MaterialData.CopyFrom(shader->GetMaterialBufferData());

		// Mark the pipeline to be remade
		m_RemakePipeline = true;

		if (m_MaterialBuffer.IsValid())
			ResourceManager::Destroy(m_MaterialBuffer);

		if (m_MaterialData.Size > 0)
			m_MaterialBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, m_MaterialData.Size);

		m_UpdateBuffer = true;
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

	float Material::GetFloat(const std::string& propertyName)
	{
		return m_MaterialData.GetValue<float>(propertyName);
	}

	float2 Material::GetFloat2(const std::string& propertyName)
	{
		return m_MaterialData.GetValue<float2>(propertyName);
	}

	float3 Material::GetFloat3(const std::string& propertyName)
	{
		return m_MaterialData.GetValue<float3>(propertyName);
	}

	float4 Material::GetFloat4(const std::string& propertyName)
	{
		return m_MaterialData.GetValue<float4>(propertyName);
	}

	bool Material::GetBool(const std::string& propertyName)
	{
		return m_MaterialData.GetValue<bool>(propertyName);
	}

	int32_t Material::GetInt32(const std::string& propertyName)
	{
		return m_MaterialData.GetValue<int32_t>(propertyName);
	}

	void Material::SetFloat(const std::string& propertyName, float value)
	{
		if (m_MaterialData.PropertyMap.contains(propertyName))
		{
			size_t index = m_MaterialData.PropertyMap[propertyName];
			size_t size = m_MaterialData.Properties[index].Size;
			size_t offset = m_MaterialData.Properties[index].Offset;

			// Write the value into the buffer
			assert(size == sizeof(float));
			m_MaterialData.Buffer.Write(&value, size, offset);
			m_UpdateBuffer = true;
		}
	}

	void Material::SetFloat2(const std::string& propertyName, float2 value)
	{
		if (m_MaterialData.PropertyMap.contains(propertyName))
		{
			size_t index = m_MaterialData.PropertyMap[propertyName];
			size_t size = m_MaterialData.Properties[index].Size;
			size_t offset = m_MaterialData.Properties[index].Offset;

			// Write the value into the buffer
			assert(size == sizeof(float2));
			m_MaterialData.Buffer.Write(&value, size, offset);
			m_UpdateBuffer = true;
		}
	}
	void Material::SetFloat3(const std::string& propertyName, float3 value)
	{
		if (m_MaterialData.PropertyMap.contains(propertyName))
		{
			size_t index = m_MaterialData.PropertyMap[propertyName];
			size_t size = m_MaterialData.Properties[index].Size;
			size_t offset = m_MaterialData.Properties[index].Offset;

			// Write the value into the buffer
			assert(size == sizeof(float3));
			m_MaterialData.Buffer.Write(&value, size, offset);
			m_UpdateBuffer = true;
		}
	}
	void Material::SetFloat4(const std::string& propertyName, float4 value)
	{
		if (m_MaterialData.PropertyMap.contains(propertyName))
		{
			size_t index = m_MaterialData.PropertyMap[propertyName];
			size_t size = m_MaterialData.Properties[index].Size;
			size_t offset = m_MaterialData.Properties[index].Offset;

			// Write the value into the buffer
			assert(size == sizeof(float4));
			m_MaterialData.Buffer.Write(&value, size, offset);
			m_UpdateBuffer = true;
		}
	}

	void Material::SetBool(const std::string& propertyName, bool value)
	{
		if (m_MaterialData.PropertyMap.contains(propertyName))
		{
			size_t index = m_MaterialData.PropertyMap[propertyName];
			size_t size = m_MaterialData.Properties[index].Size;
			size_t offset = m_MaterialData.Properties[index].Offset;

			// Write the value into the buffer
			assert(size == sizeof(bool));
			m_MaterialData.Buffer.Write(&value, size, offset);
			m_UpdateBuffer = true;
		}
	}
	void Material::SetInt32(const std::string& propertyName, int32_t value)
	{
		if (m_MaterialData.PropertyMap.contains(propertyName))
		{
			size_t index = m_MaterialData.PropertyMap[propertyName];
			size_t size = m_MaterialData.Properties[index].Size;
			size_t offset = m_MaterialData.Properties[index].Offset;

			// Write the value into the buffer
			assert(size == sizeof(int32_t));
			m_MaterialData.Buffer.Write(&value, size, offset);
			m_UpdateBuffer = true;
		}
	}
}