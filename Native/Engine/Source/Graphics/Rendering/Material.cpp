#include "Material.h"
#include "Shader.h"
#include "Texture2D.h"
#include "AssetManager.h"
#include "ryml.hpp"

namespace Odyssey
{
	Material::Material(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath)
		: Asset(assetPath, metaPath)
	{
		LoadFromDisk(assetPath);
	}

	void Material::Save()
	{
		if (!m_AssetPath.empty())
		{
			SaveMetadata();
			SaveToDisk(m_AssetPath);
		}
	}

	void Material::Load()
	{
		if (!m_AssetPath.empty())
		{
			LoadMetadata();
			LoadFromDisk(m_AssetPath);
		}
	}

	void Material::SaveToDisk(const std::filesystem::path& assetPath)
	{
		// Create a tree and root node
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		if (Shader* fragmentShader = m_FragmentShader.Get())
			root["m_FragmentShader"] << fragmentShader->GetGUID();
		if (Shader* vertexShader = m_VertexShader.Get())
			root["m_VertexShader"] << vertexShader->GetGUID();
		if (Texture2D* texture = m_Texture.Get())
			root["m_Texture"] << texture->GetGUID();

		// Save to disk
		FILE* file2 = fopen(assetPath.string().c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file2);
		fclose(file2);
	}

	void Material::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		if (std::ifstream ifs{ assetPath })
		{
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			std::string fragGUID;
			std::string vertGUID;
			std::string textureGUID;

			node["m_FragmentShader"] >> fragGUID;
			node["m_VertexShader"] >> vertGUID;
			node["m_Texture"] >> textureGUID;

			if (!fragGUID.empty())
				m_FragmentShader = AssetManager::LoadShaderByGUID(fragGUID);
			if (!vertGUID.empty())
				m_VertexShader = AssetManager::LoadShaderByGUID(vertGUID);
			if (!textureGUID.empty())
				m_Texture = AssetManager::LoadTexture2DByGUID(textureGUID);
		}
	}
}