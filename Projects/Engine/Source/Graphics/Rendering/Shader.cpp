#include "Shader.h"
#include "VulkanShaderModule.h"
#include "ResourceManager.h"
#include "AssetSerializer.h"
#include "AssetManager.h"
#include "SourceShader.h"

namespace Odyssey
{
	Shader::Shader(const Path& assetPath)
		: Asset(assetPath)
	{
		LoadFromDisk(assetPath);

		if (m_ShaderCodeBuffer)
			m_ShaderModule = ResourceManager::AllocateShaderModule(m_ShaderType, m_ShaderCodeBuffer);
	}

	Shader::Shader(const Path& assetPath, std::shared_ptr<SourceShader> source)
		: Asset(assetPath)
	{
		m_ShaderType = source->GetShaderType();

		if (source->Compile(m_ShaderCodeBuffer))
		{
			m_ShaderCodeGUID = AssetManager::CreateBinaryAsset(m_ShaderCodeBuffer);
			m_ShaderModule = ResourceManager::AllocateShaderModule(m_ShaderType, m_ShaderCodeBuffer);
		}

		SetSourceAsset(source->GetGUID());
	}

	void Shader::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			uint32_t shaderType = 0;
			std::string modulePath;

			root.ReadData("m_ShaderType", shaderType);
			m_ShaderType = (ShaderType)shaderType;

			root.ReadData("m_ShaderCode", m_ShaderCodeGUID.Ref());

			if (m_ShaderCodeGUID)
				m_ShaderCodeBuffer = AssetManager::LoadBinaryAsset(m_ShaderCodeGUID);
		}
	}

	void Shader::Recompile()
	{
		if (std::shared_ptr<SourceShader> shader = AssetManager::LoadSourceShader(m_SourceAsset))
		{
			BinaryBuffer temp;
			shader->SetShaderType(m_ShaderType);

			if (shader->Compile(temp))
			{
				m_ShaderCodeBuffer = temp;
				AssetManager::WriteBinaryAsset(m_ShaderCodeGUID, m_ShaderCodeBuffer);

				if (m_ShaderModule)
					ResourceManager::DestroyShader(m_ShaderModule);

				m_ShaderModule = ResourceManager::AllocateShaderModule(m_ShaderType, m_ShaderCodeBuffer);
			}
		}
	}

	void Shader::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Shader::Load()
	{
		LoadFromDisk(m_AssetPath);
	}

	void Shader::SaveToDisk(const std::filesystem::path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize metadata first
		SerializeMetadata(serializer);
		root.WriteData("m_ShaderType", (uint32_t)m_ShaderType);
		root.WriteData("m_ShaderCode", m_ShaderCodeGUID.CRef());

		serializer.WriteToDisk(path);
	}
}