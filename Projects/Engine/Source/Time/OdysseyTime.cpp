#include "OdysseyTime.h"

namespace Odyssey
{
	void Time::Begin()
	{
		s_Stopwatch.Start();
	}
	void Time::Tick()
	{
		s_Stopwatch.Signal();
		s_Elapsed = s_Stopwatch.Elapsed();
		s_DeltaTime = s_Stopwatch.DeltaTime();
	}
}