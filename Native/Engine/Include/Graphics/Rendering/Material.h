#pragma once
#include "Asset.h"
#include "AssetHandle.h"

namespace Odyssey
{
	class Shader;

	class Material : public Asset
	{
	public:
		Material() = default;
		Material(const std::string& path);

	public:
		void Load(const std::string& path);
		void Save(const std::string& path);

	public:
		AssetHandle<Shader> GetVertexShader() { return m_VertexShader; }
		AssetHandle<Shader> GetFragmentShader() { return m_FragmentShader; }

	public:
		void SetFragmentShader(AssetHandle<Shader> shader) { m_FragmentShader = shader; }
		void SetVertexShader(AssetHandle<Shader> shader) { m_VertexShader = shader; }
	private:
		AssetHandle<Shader> m_VertexShader;
		AssetHandle<Shader> m_FragmentShader;
	};
}