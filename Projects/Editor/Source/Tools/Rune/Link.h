#pragma once

namespace Odyssey::Rune
{
	typedef uintptr_t LinkId;

	struct Link
	{
	public:
		LinkId ID;
		int32_t StartPinID;
		int32_t EndPinID;
		float3 Color;

	public:
		Link(LinkId id, int32_t startPinID, int32_t endPinID)
			: ID(id), StartPinID(startPinID), EndPinID(endPinID), Color(1.0f)
		{

		}
	};
}