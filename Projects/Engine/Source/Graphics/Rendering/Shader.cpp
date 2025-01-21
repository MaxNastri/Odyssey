#include "Shader.h"
#include "VulkanShaderModule.h"
#include "ResourceManager.h"
#include "AssetSerializer.h"
#include "AssetManager.h"
#include "SourceShader.h"
#include "VulkanDescriptorLayout.h"
#include "spirv_cross/spirv_reflect.hpp"

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
			spirv_cross::CompilerReflection refl(shaderData.CodeBuffer. Convert<uint32_t>());
			spirv_cross::ShaderResources resources = refl.get_shader_resources();

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