#pragma once
#include "Asset.h"
#include "BinaryBuffer.h"
#include "Enums.h"

namespace Odyssey
{
	class SourceShader : public SourceAsset
	{
	public:
		SourceShader() = default;
		SourceShader(const Path& sourcePath);

	public:
		bool Compile();
		bool Compile(BinaryBuffer& codeBuffer);

	public:
		const std::string& GetShaderLanguage() { return m_ShaderLanguage; }
		bool IsCompiled() { return m_Compiled; }
		void SetShaderType(ShaderType shaderType) { m_ShaderType = shaderType; }
		ShaderType GetShaderType() { return m_ShaderType; }

	private:
		std::string ReadShaderFile(const std::filesystem::path& path);
		
	private:
		std::string m_ShaderName;
		std::string m_ShaderLanguage;
		std::string m_ShaderCode;
		ShaderType m_ShaderType;
		bool m_Compiled = false;
	};
}