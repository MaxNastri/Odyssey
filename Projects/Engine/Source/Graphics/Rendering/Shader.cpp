#include "Shader.h"
#include "VulkanShaderModule.h"
#include "ResourceManager.h"
#include "AssetSerializer.h"
#include "AssetManager.h"
#include "SourceShader.h"
#include "VulkanDescriptorLayout.h"

namespace Odyssey
{
	Shader::Shader(const Path& assetPath)
		: Asset(assetPath)
	{
		if (m_Source = AssetManager::LoadSourceAsset<SourceShader>(m_SourceAsset))
		{
			m_Source->AddOnModifiedListener([this]() { OnSourceModified(); });
			LoadAssetData();
			LoadFromSource(m_Source);
		}

		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			if (shaderData.CodeBuffer)
				shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
		}
	}

	Shader::Shader(const Path& assetPath, Ref<SourceShader> source)
		: Asset(assetPath), m_Source(source)
	{
		std::vector<ShaderType> shaderTypes = source->GetShaderTypes();
		for (ShaderType shaderType : shaderTypes)
		{
			auto& shaderData = m_Shaders[shaderType];
			if (source->Compile(shaderType, shaderData.CodeBuffer))
			{
				//shaderData.CodeGUID = AssetManager::CreateBinaryAsset(shaderData.CodeBuffer);
				shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
			}
		}

		SetSourceAsset(source->GetGUID());
	}


	void Shader::Recompile()
	{
		if (m_Source)
		{
			auto shaderTypes = m_Source->GetShaderTypes();
			for (auto shaderType : shaderTypes)
			{
				BinaryBuffer tempBuffer;
				auto& shaderData = m_Shaders[shaderType];

				if (m_Source->Compile(shaderType, tempBuffer))
				{
					shaderData.CodeBuffer = tempBuffer;

					if (shaderData.ShaderModule)
						ResourceManager::Destroy(shaderData.ShaderModule);

					shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
				}
			}
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
			LoadAssetData();
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

	void Shader::ApplyShaderBindings()
	{
		if (m_DescriptorLayout.Invalid())
			m_DescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();

		Ref<VulkanDescriptorLayout> descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(m_DescriptorLayout);
		
		for (const ShaderBinding& binding : m_Bindings)
			descriptorLayout->AddBinding(binding.Name, binding.DescriptorType, binding.Index);

		descriptorLayout->Apply();
	}

	void Shader::LoadAssetData()
	{
		AssetDeserializer deserializer(m_AssetPath);

		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			SerializationNode bindingsNode = root.GetNode("Bindings");

			assert(bindingsNode.IsSequence());

			for (size_t i = 0; i < bindingsNode.ChildCount(); i++)
			{
				SerializationNode bindingNode = bindingsNode.GetChild(i);
				assert(bindingNode.IsMap());

				std::string descriptorType;
				ShaderBinding& binding = m_Bindings.emplace_back();
				bindingNode.ReadData("Name", binding.Name);
				bindingNode.ReadData("Descriptor Type", descriptorType);
				bindingNode.ReadData("Index", binding.Index);

				assert(!descriptorType.empty());
				binding.DescriptorType = Enum::ToEnum<DescriptorType>(descriptorType);
			}
		}

		if (m_Bindings.size() > 0)
			ApplyShaderBindings();
	}

	void Shader::LoadFromSource(Ref<SourceShader> source)
	{
		// Clear any existing shaders
		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			shaderData.CodeBuffer.Clear();
			ResourceManager::Destroy(shaderData.ShaderModule);
		}
		m_Shaders.clear();

		auto shaderTypes = source->GetShaderTypes();
		for (auto shaderType : shaderTypes)
		{
			BinaryBuffer tempBuffer;
			auto& shaderData = m_Shaders[shaderType];

			if (source->Compile(shaderType, tempBuffer))
			{
				shaderData.CodeBuffer = tempBuffer;
				shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
			}
		}
	}

	void Shader::SaveToDisk(const Path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize metadata first
		SerializeMetadata(serializer);

		// Serialize the bindings
		SerializationNode bindingsNode = root.CreateSequenceNode("Bindings");

		for (size_t i = 0; i < m_Bindings.size(); i++)
		{
			ShaderBinding& binding = m_Bindings[i];
			SerializationNode bindingNode = bindingsNode.AppendChild();
			bindingNode.SetMap();

			bindingNode.WriteData("Name", binding.Name);
			bindingNode.WriteData("Descriptor Type", Enum::ToString(binding.DescriptorType));
			bindingNode.WriteData("Index", binding.Index);
		}

		serializer.WriteToDisk(path);
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