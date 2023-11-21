#pragma once
#include <memory>
#include <queue>
#include <vector>

namespace Odyssey
{
	template <typename T>
	class DynamicList
	{
	public:
		DynamicList()
		{
			m_Data.resize(initialCapacity);

			for (uint32_t i = 0; i < initialCapacity; i++)
			{
				m_FreeIndices.push(i);
			}
		}

	public:
		T* operator[](int index)
		{
			return m_Data[index].get();
		}

	public:
		template<typename... Args>
		uint32_t Add(Args... params)
		{
			if (m_FreeIndices.size() == 0)
			{
				uint32_t nextIndex = (uint32_t)m_Data.size();

				m_Data.resize(m_Data.size() * 2);

				for (; nextIndex < m_Data.size(); nextIndex++)
				{
					m_FreeIndices.push(nextIndex);
				}
			}

			uint32_t nextIndex = m_FreeIndices.front();
			m_FreeIndices.pop();

			m_Data[nextIndex] = std::make_shared<T>(params...);
			return nextIndex;
		}

		void Remove(uint32_t index)
		{
			m_Data[index].reset();
			m_FreeIndices.push(index);
		}

	private:
		const size_t initialCapacity = 64;
		std::vector<std::shared_ptr<T>> m_Data;
		std::queue<uint32_t> m_FreeIndices;
	};
}