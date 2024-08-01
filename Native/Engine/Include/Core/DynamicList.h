#pragma once
#include <memory>
#include <queue>
#include <vector>

namespace Odyssey
{
	template <typename Base>
	class DynamicList
	{
	public:
		DynamicList()
		{
			m_Data.resize(initialCapacity);

			for (size_t i = 0; i < initialCapacity; i++)
			{
				m_FreeIndices.push(i);
			}
		}

	public:
		template<typename T>
		std::shared_ptr<T> Get(size_t index)
		{
			if (index < m_Data.size() && m_Data[index])
			{
				if (std::shared_ptr<Base> basePtr = m_Data[index])
				{
					return std::static_pointer_cast<T>(basePtr);
				}
			}
			
			return nullptr;
		}

		std::shared_ptr<Base> operator[](size_t index)
		{
			return m_Data[index];
		}

	public:
		template<typename T, typename... Args>
		uint64_t Add(Args... params)
		{
			if (m_FreeIndices.size() == 0)
				Resize();

			// Pop the next free index
			size_t nextIndex = m_FreeIndices.front();
			m_FreeIndices.pop();

			// Create an entry and return the index
			m_Data[nextIndex] = std::make_shared<T>(params...);
			return nextIndex;
		}

		void Remove(size_t index)
		{
			m_Data[index].reset();
			m_FreeIndices.push(index);
		}

	private:
		void Resize()
		{
			size_t nextIndex = m_Data.size();
			m_Data.resize(m_Data.size() * 2);

			for (; nextIndex < m_Data.size(); nextIndex++)
			{
				m_FreeIndices.push(nextIndex);
			}
		}

	private:
		const size_t initialCapacity = 64;
		std::vector<std::shared_ptr<Base>> m_Data;
		std::queue<size_t> m_FreeIndices;
	};
}