#pragma once

namespace Odyssey
{
	class BinaryBuffer
	{
	public:
		BinaryBuffer() = default;
		BinaryBuffer(std::vector<uint8_t> buffer);
		BinaryBuffer(uint8_t* buffer, size_t size);

	public:
		operator bool() { return m_Size != 0; }

	public:
		void Clear();
		const std::vector<unsigned char>& GetData() { return m_Data; }
		size_t GetSize() { return m_Size; }
		size_t GetCount() { return m_Count; }

	public:
		static void CopyBuffer(BinaryBuffer& source, BinaryBuffer& destination)
		{
			destination.m_Data = source.Convert<uint8_t>();
			destination.m_Size = source.m_Size;
			destination.m_Count = source.m_Count;
		}

	public:
		template<typename T>
		std::vector<T> Convert()
		{
			size_t objSize = sizeof(T);
			size_t elements = m_Size / objSize;

			std::vector<T> buffer(elements);

			for (size_t i = 0; i < elements; i++)
			{
				size_t dataElement = i * objSize;
				memcpy(&buffer[i], &m_Data[dataElement], objSize);
			}

			return buffer;
		}

		template<typename T>
		void AppendData(std::vector<T> writeData)
		{
			// Calculate the buffer size
			size_t objSize = sizeof(T);

			size_t writeSize = objSize * writeData.size();
			size_t prevSize = m_Size;
			m_Size += writeSize;
			m_Count++;

			m_Data.resize(m_Size);
			memcpy(&(m_Data[0]) + prevSize, writeData.data(), writeSize);
		}

		template<typename T>
		void WriteData(std::vector<T> writeData)
		{
			// Calculate the buffer size
			size_t objSize = sizeof(T);

			m_Size = objSize * writeData.size();
			m_Count = writeData.size();

			// Clear the data and resize to match
			m_Data.clear();
			m_Data.resize(m_Size);

			// Mem-copy the data into the binary buffer
			memcpy(&m_Data[0], writeData.data(), m_Size);
		}

		template<typename T>
		void WriteData(T* data, size_t size)
		{
			m_Size = size;

			// Clear the data and resize to match
			m_Data.clear();
			m_Data.resize(m_Size);

			m_Count = size / sizeof(T);

			// Mem-copy the data into the binary buffer
			if (data)
				memcpy(&m_Data[0], data, m_Size);
		}

	private:
		std::vector<unsigned char> m_Data;
		size_t m_Size = 0;
		size_t m_Count = 0;
	};
}