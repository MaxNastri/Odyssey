#include "Stopwatch.h"

namespace Odyssey::Framework
{
	void Stopwatch::Start()
	{
		Restart();
		running = true;
	}

	void Stopwatch::Signal()
	{
		prevLastSignal = lastSignal;
		lastSignal = std::chrono::high_resolution_clock::now();
	}

	void Stopwatch::Stop()
	{
		stop = std::chrono::high_resolution_clock::now();
		running = false;
	}

	void Stopwatch::Restart()
	{
		start = stop = prevLastSignal = lastSignal = std::chrono::high_resolution_clock::now();
	}

	float Stopwatch::Elapsed(TimeUnit unit)
	{
		if (running)
		{
			std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
			return (float)(std::chrono::duration_cast<std::chrono::nanoseconds>(now - start).count() * timeMultipliers[unit]);
		}
		else
		{
			return (float)(std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() * timeMultipliers[unit]);
		}
	}

	float Stopwatch::DeltaTime(TimeUnit unit)
	{
		return (float)(std::chrono::duration_cast<std::chrono::nanoseconds>(lastSignal - prevLastSignal).count() * timeMultipliers[unit]);
	}
}