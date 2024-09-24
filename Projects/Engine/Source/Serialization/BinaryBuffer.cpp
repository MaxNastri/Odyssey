#include "BinaryBuffer.h"

namespace Odyssey
{
	BinaryBuffer::BinaryBuffer(std::vector<uint8_t> buffer)
	{
		WriteData(buffer);
	}

	BinaryBuffer::BinaryBuffer(uint8_t* buffer, size_t size)
	{
		WriteData(buffer, size);
	}

	void BinaryBuffer::Clear()
	{
		m_Data.clear();
		m_Size = 0;
	}
}