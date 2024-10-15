#pragma once

namespace Odyssey::Rune
{
	typedef uint64_t LinkId;

	struct Link
	{
	public:
		LinkId ID;
		uint64_t StartPinID;
		uint64_t EndPinID;
		float3 Color;

	public:
		Link(LinkId id, uint64_t startPinID, uint64_t endPinID)
			: ID(id), StartPinID(startPinID), EndPinID(endPinID), Color(1.0f)
		{

		}
	};
}