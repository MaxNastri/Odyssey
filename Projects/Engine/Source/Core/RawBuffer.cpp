#include "RawBuffer.hpp"

namespace Odyssey
{
	using Byte = uint8_t;

	RawBuffer::RawBuffer(const void* data, size_t size)
		: m_Data((void*)data), m_Size(size)
	{

	}

	RawBuffer::~RawBuffer()
	{
		Free();
	}

	Byte* RawBuffer::ReadBytes(uint64_t size, uint64_t offset) const
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

	void RawBuffer::Write(const void* data)
	{
		Write(data, m_Size);
	}

	void RawBuffer::Write(const void* data, uint64_t size, uint64_t offset)
	{
		if (size + offset > m_Size)
		{
			// TODO: LOG error
			return;
		}
		memcpy((Byte*)m_Data + offset, data, size);
	}

	void RawBuffer::Allocate(size_t size)
	{
		if (size == 0)
			return;

		Free();

		m_Size = size;

		m_Data = new Byte[m_Size];

		ZeroMemory(m_Data, size);
	}

	void RawBuffer::Free()
	{
		if (m_Data && m_Size)
		{
			delete[](byte*)m_Data;
			m_Data = nullptr;
			m_Size = 0;
		}
	}

	void RawBuffer::Copy(RawBuffer& dstBuffer, const RawBuffer& srcBuffer)
	{
		dstBuffer.Allocate(srcBuffer.m_Size);
		memcpy(dstBuffer.m_Data, srcBuffer.m_Data, dstBuffer.m_Size);
	}

	void RawBuffer::Copy(RawBuffer& dstBuffer, const void* data, uint64_t size)
	{
		dstBuffer.Allocate(size);
		dstBuffer.Write(data);
	}
}