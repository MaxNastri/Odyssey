#pragma once

namespace Odyssey
{
	class RawBuffer
	{
		using Byte = uint8_t;
	public:
		RawBuffer() = default;
		RawBuffer(const void* data, size_t size)
			: m_Data((void*)data), m_Size(size)
		{

		}

		~RawBuffer()
		{
			Free();
		}

		static RawBuffer Copy(const RawBuffer& other)
		{
			RawBuffer buffer;
			buffer.Allocate(other.m_Size);
			memcpy(buffer.m_Data, other.m_Data, other.m_Size);
			return buffer;
		}

		static RawBuffer Copy(const void* data, uint64_t size)
		{
			RawBuffer buffer;
			buffer.Allocate(size);
			memcpy(buffer.m_Data, data, size);
			return buffer;
		}

		void Allocate(size_t size)
		{
			if (size == 0)
				return;

			Free();

			m_Size = size;

			m_Data = new Byte[m_Size];

			ZeroMemory(m_Data, size);
		}
		void Free()
		{
			delete[](byte*)m_Data;
			m_Data = nullptr;
			m_Size = 0;
		}

		template<typename T>
		T& Read(uint64_t offset = 0)
		{
			return *((T*)((Byte*)m_Data + offset));
		}

		template<typename T>
		const T& Read(uint64_t offset = 0) const
		{
			return *((T*)((Byte*)m_Data + offset));
		}

		Byte* ReadBytes(uint64_t size, uint64_t offset) const
		{
			if (size + offset > m_Size)
			{
				// TODO: LOG error
				return nullptr;
			}

			Byte* buffer = new Byte[size];
			memcpy(buffer, (Byte*)m_Data + offset, size);
			return buffer;
		}

		void Write(const void* data)
		{
			Write(data, m_Size);
		}

		void Write(const void* data, uint64_t size, uint64_t offset = 0)
		{
			if (size + offset > m_Size)
			{
				// TODO: LOG error
				return;
			}
			memcpy((Byte*)m_Data + offset, data, size);
		}

		template<typename T>
		T* As() const
		{
			return (T*)m_Data;
		}

		operator bool() const
		{
			return (bool)m_Data;
		}

		inline uint64_t GetSize() const { return m_Size; }
		inline void* GetData() const { return m_Data; }
	private:
		void* m_Data = nullptr;
		size_t m_Size = 0;
	};
}