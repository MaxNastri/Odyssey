#pragma once

namespace Odyssey
{
	using Byte = uint8_t;

	class RawBuffer
	{
	public:
		RawBuffer() = default;
		RawBuffer(const void* data, size_t size);
		~RawBuffer();

	public:
		void Allocate(size_t size);
		void Free();
		Byte* ReadBytes(uint64_t size, uint64_t offset) const;
		void Write(const void* data);
		void Write(const void* data, uint64_t size, uint64_t offset = 0);

	public:
		operator bool() const { return (bool)m_Data; }
		inline uint64_t GetSize() const { return m_Size; }
		inline void* GetData() const { return m_Data; }

	public:
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

		template<typename T>
		T Read(uint64_t offset, uint64_t size)
		{
			Byte* data = ReadBytes(size, offset);
			T copy = *(T*)data;
			delete data;
			data = nullptr;
			return copy;
		}

		template<typename T>
		T* As() const
		{
			return (T*)m_Data;
		}

	public:
		static void Copy(RawBuffer& dstBuffer, const RawBuffer& srcBuffer);
		static void Copy(RawBuffer& dstBuffer, const void* data, uint64_t size);

	private:
		void* m_Data = nullptr;
		size_t m_Size = 0;
	};
}