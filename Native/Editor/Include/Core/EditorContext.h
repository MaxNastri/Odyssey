#pragma once
#include "ShaderCompiler.h"

namespace Odyssey
{
	class EditorContext
	{
	public:
		EditorContext(EditorContext const&) = delete;
		EditorContext& operator=(EditorContext const&) = delete;
		~EditorContext() {}

	private:
		explicit EditorContext();

	public:
		static EditorContext* GetInstance();

	public:
		ShaderCompiler* GetShaderCompiler() { return m_ShaderCompiler.get(); }

	private:
		std::unique_ptr<ShaderCompiler> m_ShaderCompiler;
	};
}