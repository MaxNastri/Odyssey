#pragma once
#include "Enums.h"

namespace Odyssey
{
	class ShaderCompiler
	{
	public:
		struct Options
		{
		public:
			bool Optimize = false;
		};

		struct Result
		{
		public:
			bool Success;

		};
	public:
		ShaderCompiler(Options options);

	public:
		void Compile(std::string_view shaderName, ShaderType shaderType, std::filesystem::path shader);

	private:
		std::vector<char> ReadShaderFile(std::filesystem::path& path);

	private:
		const std::string HLSL_EXTENSION = ".hlsl";
		const std::string GLSL_EXTENSION = ".glsl";

	private:
		Options m_Options;
	};
}