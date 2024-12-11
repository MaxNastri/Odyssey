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
		virtual void Save() override;
		void Load();

	private:
		void SaveToDisk(const Path& path);
		void LoadFromDisk(const Path& path);

	public:
		Ref<Shader> GetShader() { return m_Shader; }
		Ref<Texture2D> GetColorTexture() { return m_ColorTexture; }
		Ref<Texture2D> GetNormalTexture() { return m_NormalTexture; }

	public:
		void SetShader(Ref<Shader> shader) { m_Shader = shader; }
		void SetColorTexture(Ref<Texture2D> texture) { m_ColorTexture = texture; }
		void SetNormalTexture(Ref<Texture2D> texture) { m_NormalTexture = texture; }

	private:
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_ColorTexture;
		Ref<Texture2D> m_NormalTexture;
	};
}