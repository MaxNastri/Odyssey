#pragma once
#include <EventSystem.h>

namespace Odyssey::Scripting
{
	struct OnBuildStart : public Framework::Event
	{

	};

	struct OnBuildFinished : public Framework::Event
	{
	public:
		OnBuildFinished(bool succeeded)
		{
			success = succeeded;
		}

		bool success = false;
	};
}