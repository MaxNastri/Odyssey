#pragma once
#include "Enums.h"

namespace Odyssey
{
	enum ShaderLanguage
	{
		NONE = 0,
		GLSL = 1,
		HLSL = 2,
	};
	class ShaderCompiler
	{
	public:
		struct CompilerSettings
		{
		public:
			std::string ShaderName;
			ShaderType ShaderType;
			ShaderLanguage ShaderLanguage;
			std::string ShaderCode;
			bool Optimize = false;
		};

		struct Result
		{
		public:
			bool Success;
		};

	public:
		static bool Compile(const CompilerSettings settings, std::vector<uint32_t>& outByteCode);

	private:
		inline static std::string HLSL_EXTENSION = ".hlsl";
		inline static std::string GLSL_EXTENSION = ".glsl";

	private:
		inline static CompilerSettings s_Options;
	};
}