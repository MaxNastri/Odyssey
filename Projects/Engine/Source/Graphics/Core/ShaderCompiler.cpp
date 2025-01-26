#include "ShaderCompiler.h"
#include <shaderc/shaderc.hpp>
#include <fstream>
#include <filesystem>
#include "Log.h"

namespace Odyssey
{
	bool ShaderCompiler::Compile(const CompilerSettings settings, BinaryBuffer& codeBuffer)
	{
		// HLSL Shader
		shaderc::Compiler compiler;
		shaderc::CompileOptions compilerOptions;

		switch (settings.ShaderLanguage)
		{
		case ShaderLanguage::NONE:
			Log::Error("[ShaderCompiler] Invalid shader language detected: " + settings.ShaderName);
			break;
		case ShaderLanguage::GLSL:
			compilerOptions.SetSourceLanguage(shaderc_source_language_glsl);
			break;
		case ShaderLanguage::HLSL:
			compilerOptions.SetSourceLanguage(shaderc_source_language_hlsl);
			break;
		default:
			break;
		}
		if (s_Options.Optimize)
			compilerOptions.SetOptimizationLevel(shaderc_optimization_level_size);

		auto type = settings.ShaderType == ShaderType::Vertex ? shaderc_vertex_shader : shaderc_fragment_shader;
		if (settings.ShaderType == ShaderType::Compute)
			type = shaderc_compute_shader;
		if (settings.ShaderType == ShaderType::Geometry)
			type = shaderc_geometry_shader;
		if (settings.ShaderType == ShaderType::Hull)
			type = shaderc_tess_control_shader;
		if (settings.ShaderType == ShaderType::Domain)
			type = shaderc_tess_evaluation_shader;

		shaderc::SpvCompilationResult result =
			compiler.CompileGlslToSpv(settings.ShaderCode, type, settings.ShaderName.c_str(), compilerOptions);

		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			Log::Error("[ShaderCompiler] Failed to compile shader: " + settings.ShaderName);
			Log::Error("[ShaderCompiler] " + result.GetErrorMessage());
			return false;
		}

		// Store the bytecode in the out parameter and return success
		codeBuffer.WriteData(std::vector(result.begin(), result.end()));
		return true;
	}
}