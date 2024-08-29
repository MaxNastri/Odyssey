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
		bool Compile(ShaderType shaderType, BinaryBuffer& codeBuffer);

	public:
		const std::string& GetShaderLanguage() { return m_ShaderLanguage; }
		bool IsCompiled() { return m_Compiled; }
		std::vector<ShaderType> GetShaderTypes();

	private:
		void ParseShaderFile(const Path& path);

	private:
		std::string m_ShaderName;
		std::string m_ShaderLanguage;
		std::string m_VertexShaderCode;
		std::string m_FragmentShaderCode;
		std::map<ShaderType, std::string> m_ShaderCode;
		bool m_Compiled = false;
	};
}