#pragma once
#include "Stopwatch.h"

namespace Odyssey
{
	class Time
	{
	public:
		static void Begin();
		static void Tick();

	public:
		static float Elapsed() { return s_Elapsed; }
		static float DeltaTime() { return s_DeltaTime; }

	public:
		inline static float s_DeltaTime;
		inline static float s_Elapsed;
		inline static Stopwatch s_Stopwatch;

	};
}