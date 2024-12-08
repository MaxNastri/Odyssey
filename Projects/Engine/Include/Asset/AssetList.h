#pragma once
#include "Asset.h"
#include "FreeList.h"
#include "Ref.h"

namespace Odyssey
{
	class AssetList
	{
	public:
		AssetList() = default;

	public:
		template<typename T, typename... Args>
		Ref<T> Add(GUID guid, Args... params)
		{
			uint64_t index = m_Assets.Add<T>(std::forward<Args>(params)...);
			Ref<T> asset = m_Assets.Get<T>(index);
			m_AssetToIndex[guid] = index;

			return asset;
		}

		Ref<Asset> Get(GUID guid)
		{
			Ref<Asset> value = nullptr;

			if (m_AssetToIndex.contains(guid))
				value = m_Assets[m_AssetToIndex[guid]];

			return value;
		}

		template<typename T>
		Ref<T> Get(GUID guid)
		{
			if (Ref<Asset> value = Get(guid))
				return value.As<T>();

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
		Ref<T> Add(GUID guid, Args... params)
		{
			uint64_t index = m_Assets.Add<T>(std::forward<Args>(params)...);
			Ref<T> asset = m_Assets.Get<T>(index);
			m_AssetToIndex[guid] = index;

			return asset;
		}

		void Remove(GUID guid)
		{
			if (m_AssetToIndex.contains(guid))
				m_Assets.Remove(m_AssetToIndex[guid]);
		}

		Ref<SourceAsset> Get(GUID guid)
		{
			Ref<SourceAsset> value = nullptr;

			if (m_AssetToIndex.contains(guid))
				value = m_Assets[m_AssetToIndex[guid]];

			return value;
		}

		template<typename T>
		Ref<T> Get(GUID guid)
		{
			if (Ref<SourceAsset> value = Get(guid))
				return value.As<T>();

			return nullptr;
		}
	private:
		FreeList<SourceAsset> m_Assets;
		std::map<GUID, size_t> m_AssetToIndex;
	};
}