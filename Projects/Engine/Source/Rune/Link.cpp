#include "Link.h"

namespace Odyssey::Rune
{
	Link::Link(GUID startPinGUID, GUID endPinGUID)
		: Guid(GUID::New()), StartPinGUID(startPinGUID), EndPinGUID(endPinGUID), Color(1.0f)
	{

	}
}