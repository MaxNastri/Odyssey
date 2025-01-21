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
		{
			LoadFromSource(m_Source);
		}
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

	void Shader::LoadFromSource(Ref<SourceShader> source)
	{
		// Clear any existing shaders
		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			shaderData.CodeBuffer.Clear();
			ResourceManager::Destroy(shaderData.ShaderModule);
		}

		Recompile();
	}

	void Shader::SaveToDisk(const Path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize metadata first
		SerializeMetadata(serializer);
		serializer.WriteToDisk(path);
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
					inputDesc.location = i;
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

	void Shader::GenerateVertexLayout()
	{
		//std::vector<VkVertexInputAttributeDescription> descriptions;

		//// Position
		//auto& positionDesc = descriptions.emplace_back();
		//positionDesc.binding = 0;
		//positionDesc.location = 0;
		//positionDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		//positionDesc.offset = offsetof(Vertex, Position);

		//// Normal
		//auto& normalDesc = descriptions.emplace_back();
		//normalDesc.binding = 0;
		//normalDesc.location = 1;
		//normalDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		//normalDesc.offset = offsetof(Vertex, Normal);

		//if (skinned)
		//{
		//	// Bone Indices
		//	auto& indicesDesc = descriptions.emplace_back();
		//	indicesDesc.binding = 0;
		//	indicesDesc.location = 2;
		//	indicesDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		//	indicesDesc.offset = offsetof(Vertex, BoneIndices);

		//	// Bone Weights
		//	auto& weightsDesc = descriptions.emplace_back();
		//	weightsDesc.binding = 0;
		//	weightsDesc.location = 3;
		//	weightsDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		//	weightsDesc.offset = offsetof(Vertex, BoneWeights);
		//}

		//attributeDescriptions.WriteData(descriptions);
	}

	void Shader::OnSourceModified()
	{
		if (m_Source)
		{
			m_Source->Reload();
			LoadFromSource(m_Source);

			for (std::function<void()> callback : m_OnModifiedListeners)
				callback();
		}
	}
}