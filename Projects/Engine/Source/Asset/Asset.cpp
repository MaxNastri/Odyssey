#include "Asset.h"
#include "AssetSerializer.h"
#include "AssetManager.h"
#include "FileManager.h"

namespace Odyssey
{
	SourceAsset::SourceAsset(const Path& sourcePath)
	{
		m_SourcePath = sourcePath;
		m_SourceExtension = sourcePath.extension().string();

		FileActionCallback callback = [this](const Path& oldPath, const Path& newPath, FileActionType fileAction) { OnSourceModified(oldPath, newPath, fileAction); };
		m_TrackingIDs.push_back(FileManager::Get().TrackFile(sourcePath, callback));
	}

	SourceAsset::~SourceAsset()
	{
		for (auto& trackingID : m_TrackingIDs)
			FileManager::Get().UntrackFile(trackingID);
	}

	bool SourceAsset::HasMetadata()
	{
		return Guid != 0 && !Name.empty() && !Type.empty() && !m_SourceExtension.empty();
	}

	void SourceAsset::SetMetadata(GUID guid, const std::string& name, const std::string& type)
	{
		Guid = guid;
		Name = name;
		Type = type;
	}

	void SourceAsset::AddDependency(const Path& path)
	{
		// Track the dependency's file
		FileActionCallback callback = [this](const Path& oldPath, const Path& newPath, FileActionType fileAction) { OnSourceModified(oldPath, newPath, fileAction); };
		TrackingID id = FileManager::Get().TrackFile(path, callback);

		// Store the dependency
		m_Dependencies.push_back(path);
		m_TrackingIDs.push_back(id);
	}

	void SourceAsset::OnSourceModified(const Path& oldPath, const Path& newPath, FileActionType fileAction)
	{
		if (fileAction == FileActionType::Modified && (oldPath == m_SourcePath || newPath == m_SourcePath))
		{
			for (auto& callback : m_OnSourceModified)
			{
				callback();
			}
		}
	}

	Asset::Asset(const Path& assetPath)
	{
		m_AssetPath = assetPath;
		Load();
	}

	void Asset::SerializeMetadata(AssetSerializer& serializer)
	{
		SerializationNode& root = serializer.GetRoot();
		root.WriteData("m_SourceAsset", m_SourceAsset.CRef());
		root.WriteData("m_GUID", m_GUID.CRef());
		root.WriteData("m_Name", m_Name);
		root.WriteData("m_Type", m_Type);
	}

	void Asset::Load()
	{
		AssetDeserializer deserializer(m_AssetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			root.ReadData("m_SourceAsset", m_SourceAsset.Ref());
			root.ReadData("m_GUID", m_GUID.Ref());
			root.ReadData("m_Name", m_Name);
			root.ReadData("m_Type", m_Type);
		}
	}

	void Asset::SetName(std::string_view name)
	{
		m_Name = name;
		AssetManager::UpdateAssetName(m_GUID, m_Name);
	}

	void Asset::SetAssetPath(const Path& path)
	{
		m_AssetPath = path;
		AssetManager::UpdateAssetPath(m_GUID, m_AssetPath);
	}
}