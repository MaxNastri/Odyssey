#include "Link.h"
#include "RuneInternal.h"

namespace Odyssey::Rune
{
	Link::Link(uint64_t startPinID, uint64_t endPinID)
		: ID(GetNextID()), StartPinID(startPinID), EndPinID(endPinID), Color(1.0f)
	{

	}
}