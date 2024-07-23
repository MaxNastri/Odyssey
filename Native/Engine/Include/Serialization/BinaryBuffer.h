#pragma once

namespace Odyssey
{
	class BinaryBuffer
	{
	public:
		BinaryBuffer() = default;
		BinaryBuffer(std::vector<unsigned char> buffer, size_t size);

	public:
		const std::vector<unsigned char>& GetData() { return m_Data; }
		size_t GetSize() { return m_Size; }
		operator bool() { return m_Size != 0; }

	public:
		template<typename T>
		std::vector<T> Convert()
		{
			size_t objSize = sizeof(T);
			size_t elements = m_Size / objSize;

			std::vector<T> buffer(objSize * elements);

			for (size_t i = 0; i < elements; i++)
			{
				size_t dataElement = i * objSize;
				memcpy(&buffer[i], &m_Data[dataElement], objSize);
			}

			return buffer;
		}

		template<typename T>
		void WriteData(std::vector<T> writeData)
		{
			// Calculate the buffer size
			size_t objSize = sizeof(T);

			// Note: We add extra padding to store the size of the buffer
			m_Size = objSize * writeData.size();

			// Clear the data and resize to match
			m_Data.clear();
			m_Data.resize(m_Size);

			// Mem-copy the data into the binary buffer
			memcpy(&m_Data[0], writeData.data(), m_Size);
		}

	private:
		std::vector<unsigned char> m_Data;
		size_t m_Size = 0;
	};
}