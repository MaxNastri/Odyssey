#pragma once
#include "Asset.h"
#include "AssetHandle.h"

namespace Odyssey
{
	class Shader;
	class Texture2D;

	class Material : public Asset
	{
	public:
		Material() = default;
		Material(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath);

	public:
		void Save();
		void Load();

	private:
		void SaveToDisk(const std::filesystem::path& path);
		void LoadFromDisk(const std::filesystem::path& path);

	public:
		AssetHandle<Shader> GetVertexShader() { return m_VertexShader; }
		AssetHandle<Shader> GetFragmentShader() { return m_FragmentShader; }
		AssetHandle<Texture2D> GetTexture() { return m_Texture; }

	public:
		void SetFragmentShader(AssetHandle<Shader> shader) { m_FragmentShader = shader; }
		void SetVertexShader(AssetHandle<Shader> shader) { m_VertexShader = shader; }
		void SetTexture(AssetHandle<Texture2D> texture) { m_Texture = texture; }
	private:
		AssetHandle<Shader> m_VertexShader;
		AssetHandle<Shader> m_FragmentShader;
		AssetHandle<Texture2D> m_Texture;
	};
}