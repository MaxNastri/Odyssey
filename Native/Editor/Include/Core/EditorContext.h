#pragma once
#include "ShaderCompiler.h"
#include "GUIManager.h"

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
		GUIManager* GetGUIManager() { return m_GUIManager.get(); }

	private:
		std::unique_ptr<ShaderCompiler> m_ShaderCompiler;
		std::unique_ptr<GUIManager> m_GUIManager;
	};
}