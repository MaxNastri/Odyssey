#include "BinaryBuffer.h"

namespace Odyssey
{
	BinaryBuffer::BinaryBuffer(std::vector<unsigned char> buffer, size_t size)
		: m_Data(buffer), m_Size(size)
	{

	}
}