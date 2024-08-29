#pragma once
#include "Asset.h"

namespace Odyssey
{
	class Shader;
	class Texture2D;

	class Material : public Asset
	{
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
		std::shared_ptr<Shader> GetShader() { return m_Shader; }
		std::shared_ptr<Texture2D> GetTexture() { return m_Texture; }

	public:
		void SetShader(std::shared_ptr<Shader> shader) { m_Shader = shader; }
		void SetTexture(std::shared_ptr<Texture2D> texture) { m_Texture = texture; }

	private:
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<Texture2D> m_Texture;
	};
}