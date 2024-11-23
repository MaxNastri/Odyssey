#pragma once
#include "Asset.h"
#include "Ref.h"
#include "Shader.h"
#include "Texture2D.h"

namespace Odyssey
{
	class Material : public Asset
	{
		CLASS_DECLARATION(Odyssey, Material)
	public:
		Material() = default;
		Material(const Path& assetPath);

	public:
		void Save();
		void Load();

	private:
		void SaveToDisk(const Path& path);
		void LoadFromDisk(const Path& path);

	public:
		Ref<Shader> GetShader() { return m_Shader; }
		Ref<Texture2D> GetTexture() { return m_Texture; }

	public:
		void SetShader(Ref<Shader> shader) { m_Shader = shader; }
		void SetTexture(Ref<Texture2D> texture) { m_Texture = texture; }

	private:
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_Texture;
	};
}