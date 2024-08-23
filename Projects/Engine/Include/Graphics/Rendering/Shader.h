#pragma once
#include "Enums.h"
#include "Asset.h"
#include "Resource.h"
#include "BinaryBuffer.h"

namespace Odyssey
{
	class VulkanShaderModule;
	class SourceShader;

	class Shader : public Asset
	{
	public:
		Shader() = default;
		Shader(const Path& assetPath);
		Shader(const Path& assetPath, std::shared_ptr<SourceShader> source);

	public:
		void Recompile();

	public:
		ResourceID GetShaderModule() { return m_ShaderModule; }
		GUID GetShaderCodeGUID() { return m_ShaderCodeGUID; }
		ShaderType GetShaderType() { return m_ShaderType; }

	public:
		void Save();
		void Load();

	private:
		void LoadFromDisk(const Path& path);
		void SaveToDisk(const Path& path);

	private:
		ShaderType m_ShaderType = ShaderType::None;
		GUID m_ShaderCodeGUID = 0;
		BinaryBuffer m_ShaderCodeBuffer;
		ResourceID m_ShaderModule;
	};
}