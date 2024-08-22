#pragma once
#include "Asset.h"
#include "FreeList.hpp"

namespace Odyssey
{
	class AssetList
	{
	public:
		AssetList() = default;

	public:
		template<typename T, typename... Args>
		std::shared_ptr<T> Add(GUID guid, Args... params)
		{
			uint64_t index = m_Assets.Add<T>(std::forward<Args>(params)...);
			std::shared_ptr<T> asset = m_Assets.Get<T>(index);
			m_AssetToIndex[guid] = index;

			return asset;
		}

		std::shared_ptr<Asset> Get(GUID guid)
		{
			std::shared_ptr<Asset> value = nullptr;

			if (m_AssetToIndex.contains(guid))
				value = m_Assets[m_AssetToIndex[guid]];

			return value;
		}

		template<typename T>
		std::shared_ptr<T> Get(GUID guid)
		{
			if (std::shared_ptr<Asset> value = Get(guid))
				return std::static_pointer_cast<T>(value);

			return nullptr;
		}

		void Remove(GUID guid)
		{
			if (m_AssetToIndex.contains(guid))
				m_Assets.Remove(m_AssetToIndex[guid]);
		}

	private:
		FreeList<Asset> m_Assets;
		std::map<GUID, size_t> m_AssetToIndex;
	};

	class SourceAssetList
	{
	public:
		SourceAssetList() = default;

	public:
		template<typename T, typename... Args>
		std::shared_ptr<T> Add(GUID guid, Args... params)
		{
			uint64_t index = m_Assets.Add<T>(std::forward<Args>(params)...);
			std::shared_ptr<T> asset = m_Assets.Get<T>(index);
			m_AssetToIndex[guid] = index;

			return asset;
		}

		void Remove(GUID guid)
		{
			if (m_AssetToIndex.contains(guid))
				m_Assets.Remove(m_AssetToIndex[guid]);
		}

		std::shared_ptr<SourceAsset> Get(GUID guid)
		{
			std::shared_ptr<SourceAsset> value = nullptr;

			if (m_AssetToIndex.contains(guid))
				value = m_Assets[m_AssetToIndex[guid]];

			return value;
		}

		template<typename T>
		std::shared_ptr<T> Get(GUID guid)
		{
			if (std::shared_ptr<SourceAsset> value = Get(guid))
				return std::static_pointer_cast<T>(value);

			return nullptr;
		}
	private:
		FreeList<SourceAsset> m_Assets;
		std::map<GUID, size_t> m_AssetToIndex;
	};
}