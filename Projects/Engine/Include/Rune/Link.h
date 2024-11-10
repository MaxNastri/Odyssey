#pragma once
#include "Pin.h"
#include "GUID.h"

namespace Odyssey::Rune
{
	struct Link
	{
	public:
		GUID Guid;
		GUID StartPinGUID;
		GUID EndPinGUID;
		float3 Color;

	public:
		Link(GUID startPinGUID, GUID endPinGUID);
	};
}