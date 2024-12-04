#pragma once
#include "Enums.h"
#include "Asset.h"
#include "Resource.h"
#include "BinaryBuffer.h"
#include "SourceShader.h"

namespace Odyssey
{
	class VulkanShaderModule;
	class SourceShader;

	class Shader : public Asset
	{
	public:
		CLASS_DECLARATION(Odyssey, Shader)
	public:
		Shader() = default;
		Shader(const Path& assetPath);
		Shader(const Path& assetPath, Ref<SourceShader> source);

	public:
		void Recompile();

	public:
		void Save();
		void Load();

	public:
		std::map<ShaderType, ResourceID> GetResourceMap();

	private:
		void LoadFromSource(Ref<SourceShader> source);
		void SaveToDisk(const Path& path);

	private:
		void OnSourceModified();

	private:
		struct ShaderData
		{
			GUID CodeGUID;
			BinaryBuffer CodeBuffer;
			ResourceID ShaderModule;
		};
		
		std::map<ShaderType, ShaderData> m_Shaders;
		Ref<SourceShader> m_Source;
	};
}