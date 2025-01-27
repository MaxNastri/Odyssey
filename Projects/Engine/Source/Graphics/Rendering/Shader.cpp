#include "Shader.h"
#include "VulkanShaderModule.h"
#include "ResourceManager.h"
#include "AssetSerializer.h"
#include "AssetManager.h"
#include "SourceShader.h"
#include "VulkanDescriptorLayout.h"
#include "spirv_cross/spirv_reflect.hpp"
#include "Vertex.h"

namespace Odyssey
{
	Shader::Shader(const Path& assetPath)
		: Asset(assetPath)
	{
		if (m_Source = AssetManager::LoadSourceAsset<SourceShader>(m_SourceAsset))
		{
			m_Source->AddOnModifiedListener([this]() { OnSourceModified(); });
			LoadFromSource(m_Source);
		}
	}

	Shader::Shader(const Path& assetPath, Ref<SourceShader> source)
		: Asset(assetPath), m_Source(source)
	{
		Recompile();
		SetSourceAsset(source->GetGUID());
	}

	void Shader::Recompile()
	{
		if (m_Source)
		{
			m_Shaders.clear();

			std::vector<ShaderType> shaderTypes = m_Source->GetShaderTypes();

			for (ShaderType shaderType : shaderTypes)
			{
				BinaryBuffer tempBuffer;
				auto& shaderData = m_Shaders[shaderType];

				// Compile the shader
				if (m_Source->Compile(shaderType, tempBuffer))
				{
					// Destroy the previous shader module, if loaded
					if (shaderData.ShaderModule)
						ResourceManager::Destroy(shaderData.ShaderModule);

					// Load a new shader module with the updated code
					shaderData.CodeBuffer = tempBuffer;
					shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
				}
			}

			// Reflect the shader code and generate the shader resources
			GenerateShaderResources();
		}
	}

	void Shader::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Shader::Load()
	{
		if (m_Source)
			LoadFromSource(m_Source);
	}

	std::map<ShaderType, ResourceID> Shader::GetResourceMap()
	{
		std::map<ShaderType, ResourceID> resourceMap;

		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			resourceMap[shaderType] = shaderData.ShaderModule;
		}

		return resourceMap;
	}

	bool Shader::HasBinding(std::string bindingName, uint32_t& index)
	{
		if (m_Bindings.contains(bindingName))
		{
			index = m_Bindings[bindingName].Index;
			return true;
		}

		return false;
	}

	void Shader::RemoveOnModifiedListener(uint32_t listenerID)
	{
		for (size_t i = 0; i < m_OnModifiedListeners.size(); i++)
		{
			if (m_OnModifiedListeners[i].ID == listenerID)
			{
				m_OnModifiedListeners.erase(m_OnModifiedListeners.begin() + i);
				break;
			}
		}
	}

	uint32_t Shader::AddOnModifiedListener(std::function<void()> callback)
	{
		Listener& listener = m_OnModifiedListeners.emplace_back();
		listener.ID = m_NextID++;
		listener.Callback = callback;

		return listener.ID;
	}

	void Shader::LoadFromSource(Ref<SourceShader> source)
	{
		// Clear any existing shaders
		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			shaderData.CodeBuffer.Clear();
			ResourceManager::Destroy(shaderData.ShaderModule);
		}

		Recompile();
		LoadMaterialDefaults();
	}

	void Shader::SaveToDisk(const Path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize metadata first
		SerializeMetadata(serializer);

		SerializationNode propertiesNode = root.CreateSequenceNode("Properties");
		for (MaterialProperty& materialProperty : m_MaterialBufferData.Properties)
		{
			SerializationNode propertyNode = propertiesNode.AppendChild();
			propertyNode.SetMap();
			propertyNode.WriteData("Name", materialProperty.Name);
			propertyNode.WriteData("Type", Enum::ToString(materialProperty.Type));

			switch (materialProperty.Type)
			{
				case PropertyType::Float:
					propertyNode.WriteData("Default Value", m_MaterialBufferData.GetValue<float>(materialProperty.Name));
					break;
				case PropertyType::Float2:
					propertyNode.WriteData("Default Value", m_MaterialBufferData.GetValue<float2>(materialProperty.Name));
					break;
				case PropertyType::Float3:
					propertyNode.WriteData("Default Value", m_MaterialBufferData.GetValue<float3>(materialProperty.Name));
					break;
				case PropertyType::Float4:
					propertyNode.WriteData("Default Value", m_MaterialBufferData.GetValue<float4>(materialProperty.Name));
					break;
				case PropertyType::Bool:
					propertyNode.WriteData("Default Value", m_MaterialBufferData.GetValue<bool>(materialProperty.Name));
					break;
				case PropertyType::Int32:
					propertyNode.WriteData("Default Value", m_MaterialBufferData.GetValue<int32_t>(materialProperty.Name));
					break;
				default:
					break;
			}
		}

		serializer.WriteToDisk(path);
	}

	void Shader::LoadMaterialDefaults()
	{
		AssetDeserializer deserializer(m_AssetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
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

					if (m_MaterialBufferData.PropertyMap.contains(name))
					{
						switch (propertyType)
						{
							case Odyssey::PropertyType::Float:
							{
								float serializedValue;
								propertyNode.ReadData("Default Value", serializedValue);
								m_MaterialBufferData.SetValue(name, &serializedValue);
								break;
							}
							case Odyssey::PropertyType::Float2:
							{
								float2 serializedValue;
								propertyNode.ReadData("Default Value", serializedValue);
								m_MaterialBufferData.SetValue(name, &serializedValue);
								break;
							}
							case Odyssey::PropertyType::Float3:
							{
								float3 serializedValue;
								propertyNode.ReadData("Default Value", serializedValue);
								m_MaterialBufferData.SetValue(name, &serializedValue);
								break;
							}
							case Odyssey::PropertyType::Float4:
							{
								float4 serializedValue;
								propertyNode.ReadData("Default Value", serializedValue);
								m_MaterialBufferData.SetValue(name, &serializedValue);
								break;
							}
							case Odyssey::PropertyType::Bool:
							{
								bool serializedValue;
								propertyNode.ReadData("Default Value", serializedValue);
								m_MaterialBufferData.SetValue(name, &serializedValue);
								break;
							}
							case Odyssey::PropertyType::Int32:
							{
								int32_t serializedValue;
								propertyNode.ReadData("Default Value", serializedValue);
								m_MaterialBufferData.SetValue(name, &serializedValue);
								break;
							}
						}
					}
				}
			}
		}
	}

	VkFormat GetVertexFormat(spirv_cross::SPIRType inputType)
	{
		switch (inputType.basetype)
		{
			case spirv_cross::SPIRType::Float:
			{
				if (inputType.vecsize == 1)
					return VK_FORMAT_R32_SFLOAT;
				else if (inputType.vecsize == 2)
					return VK_FORMAT_R32G32_SFLOAT;
				else if (inputType.vecsize == 3)
					return VK_FORMAT_R32G32B32_SFLOAT;
				else if (inputType.vecsize == 4)
					return VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			}
			case spirv_cross::SPIRType::UInt:
			{
				if (inputType.vecsize == 1)
					return VK_FORMAT_R32_UINT;
				else if (inputType.vecsize == 2)
					return VK_FORMAT_R32G32_UINT;
				else if (inputType.vecsize == 3)
					return VK_FORMAT_R32G32B32_UINT;
				else if (inputType.vecsize == 4)
					return VK_FORMAT_R32G32B32A32_UINT;
				break;
			}
			case spirv_cross::SPIRType::UInt64:
			{
				if (inputType.vecsize == 1)
					return VK_FORMAT_R64_UINT;
				else if (inputType.vecsize == 2)
					return VK_FORMAT_R64G64_UINT;
				else if (inputType.vecsize == 3)
					return VK_FORMAT_R64G64B64_UINT;
				else if (inputType.vecsize == 4)
					return VK_FORMAT_R64G64B64A64_UINT;
				break;
			}
			case spirv_cross::SPIRType::Int:
			{
				if (inputType.vecsize == 1)
					return VK_FORMAT_R32_SINT;
				else if (inputType.vecsize == 2)
					return VK_FORMAT_R32G32_SINT;
				else if (inputType.vecsize == 3)
					return VK_FORMAT_R32G32B32_SINT;
				else if (inputType.vecsize == 4)
					return VK_FORMAT_R32G32B32A32_SINT;
				break;
			}
			case spirv_cross::SPIRType::Int64:
			{
				if (inputType.vecsize == 1)
					return VK_FORMAT_R64_SINT;
				else if (inputType.vecsize == 2)
					return VK_FORMAT_R64G64_SINT;
				else if (inputType.vecsize == 3)
					return VK_FORMAT_R64G64B64_SINT;
				else if (inputType.vecsize == 4)
					return VK_FORMAT_R64G64B64A64_SINT;
				break;
			}
			default:
				break;
		}

		assert(0);
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	}

	void Shader::GenerateShaderResources()
	{
		// Destroy the previous layout, if it exists
		if (m_DescriptorLayout)
			ResourceManager::Destroy(m_DescriptorLayout);

		m_Bindings.clear();

		// Create a new layout
		m_DescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();
		Ref<VulkanDescriptorLayout> descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(m_DescriptorLayout);

		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			// Parse the spirv code so it can be reflected
			spirv_cross::CompilerReflection refl(shaderData.CodeBuffer.Convert<uint32_t>());
			spirv_cross::ShaderResources resources = refl.get_shader_resources();

			if (shaderType == ShaderType::Vertex)
			{
				// BUild vertex attribute descriptions from the inputs
				std::vector<VkVertexInputAttributeDescription> descriptions;

				for (size_t i = 0; i < resources.stage_inputs.size(); i++)
				{
					spirv_cross::Resource& input = resources.stage_inputs[i];
					spirv_cross::SPIRType inputType = refl.get_type(input.type_id);
					std::string inputName = Odyssey::ToLower(input.name);

					std::string inputTag = "input.";
					size_t inputPos = inputName.find("input.");
					if (inputPos != std::string::npos)
						inputName = inputName.substr(inputPos + inputTag.length());

					VkVertexInputAttributeDescription& inputDesc = descriptions.emplace_back();
					inputDesc.binding = 0;
					inputDesc.location = (uint32_t)i;
					inputDesc.format = GetVertexFormat(inputType);
					inputDesc.offset = Vertex::GetOffset(inputName);
				}

				if (descriptions.size() > 0)
					m_VertexAttributes.WriteData(descriptions);
			}

			// Reflect texture sampler bindings
			for (spirv_cross::Resource& resource : resources.sampled_images)
			{
				std::string name = refl.get_name(resource.base_type_id);
				uint32_t set = refl.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32_t binding = refl.get_decoration(resource.id, spv::DecorationBinding);

				if (!m_Bindings.contains(name))
				{
					ShaderBinding& bindingData = m_Bindings[name];
					bindingData.Name = name;
					bindingData.DescriptorType = DescriptorType::Sampler;
					bindingData.Index = binding;

					descriptorLayout->AddBinding(name, DescriptorType::Sampler, binding);
				}
			}

			// Reflect sampler state bindings (texture cubes, etc)
			for (spirv_cross::Resource& resource : resources.separate_samplers)
			{
				std::string name = refl.get_name(resource.id);
				uint32_t set = refl.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32_t binding = refl.get_decoration(resource.id, spv::DecorationBinding);

				if (!m_Bindings.contains(name))
				{
					ShaderBinding& bindingData = m_Bindings[name];
					bindingData.Name = name;
					bindingData.DescriptorType = DescriptorType::Sampler;
					bindingData.Index = binding;

					descriptorLayout->AddBinding(name, DescriptorType::Sampler, binding);
				}
			}

			// Reflect uniform buffer bindings
			for (spirv_cross::Resource& resource : resources.uniform_buffers)
			{
				std::string name = refl.get_name(resource.base_type_id);
				uint32_t set = refl.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32_t binding = refl.get_decoration(resource.id, spv::DecorationBinding);

				if (name == "MaterialData" && m_MaterialBufferData.Properties.size() == 0)
				{
					spirv_cross::SPIRType type = refl.get_type(resource.base_type_id);
					size_t memberCount = type.member_types.size();

					std::vector<MaterialProperty> properties;

					for (size_t i = 0; i < type.member_types.size(); i++)
					{
						MaterialProperty& materialProperty = properties.emplace_back();
						materialProperty.Name = refl.get_member_name(type.self, (uint32_t)i);

						// Get member offset and size within this struct.
						materialProperty.Offset = refl.type_struct_member_offset(type, (uint32_t)i);
						materialProperty.Size = refl.get_declared_struct_member_size(type, (uint32_t)i);

						auto& memberType = refl.get_type(type.member_types[i]);

						switch (memberType.basetype)
						{
							case spirv_cross::SPIRType::Float:
								if (memberType.vecsize == 1)
									materialProperty.Type = PropertyType::Float;
								else if (memberType.vecsize == 2)
									materialProperty.Type = PropertyType::Float2;
								else if (memberType.vecsize == 3)
									materialProperty.Type = PropertyType::Float3;
								else if (memberType.vecsize == 4)
									materialProperty.Type = PropertyType::Float4;
								break;
							case spirv_cross::SPIRType::Boolean:
								materialProperty.Type = PropertyType::Bool;
							case spirv_cross::SPIRType::Int:
								materialProperty.Type = PropertyType::Int32;
								break;
							default:
								materialProperty.Type = PropertyType::Unknown;
								break;
						}

						//if (!memberType.array.empty())
						//{
						//	// Get array stride, e.g. float4 foo[]; Will have array stride of 16 bytes.
						//	size_t array_stride = refl.type_struct_member_array_stride(type, i);
						//}
						//
						//if (memberType.columns > 1)
						//{
						//	// Get bytes stride between columns (if column major), for float4x4 -> 16 bytes.
						//	size_t matrix_stride = refl.type_struct_member_matrix_stride(type, i);
						//}
					}

					// Set the material properties into the data
					m_MaterialBufferData.Set(properties, refl.get_declared_struct_size(type));
				}
				
				if (!m_Bindings.contains(name))
				{
					ShaderBinding& bindingData = m_Bindings[name];
					bindingData.Name = name;
					bindingData.DescriptorType = DescriptorType::Uniform;
					bindingData.Index = binding;
					descriptorLayout->AddBinding(name, DescriptorType::Uniform, binding);
				}
			}

			// Reflect storage buffer bindings
			for (spirv_cross::Resource& resource : resources.storage_buffers)
			{
				std::string name = refl.get_name(resource.id);
				uint32_t set = refl.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32_t binding = refl.get_decoration(resource.id, spv::DecorationBinding);

				if (!m_Bindings.contains(name))
				{
					ShaderBinding& bindingData = m_Bindings[name];
					bindingData.Name = name;
					bindingData.DescriptorType = DescriptorType::Storage;
					bindingData.Index = binding;
					descriptorLayout->AddBinding(name, DescriptorType::Storage, binding);
				}
			}
		}

		descriptorLayout->Apply();
	}

	void Shader::OnSourceModified()
	{
		if (m_Source)
		{
			m_Source->Reload();
			LoadFromSource(m_Source);

			for (Listener& listener : m_OnModifiedListeners)
				listener.Callback();
		}
	}
}