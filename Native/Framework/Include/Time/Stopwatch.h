#pragma once

namespace Odyssey::Framework
{
	class Stopwatch
	{
        enum TimeUnit
        {
            Seconds = 0,
            Milliseconds = 1,
            Microseconds = 2,
            Nanoseconds = 3,
        };

    public:
        void Start();
        void Signal();
        void Stop();
        void Restart();
        float Elapsed(TimeUnit unit = TimeUnit::Seconds);
        float DeltaTime(TimeUnit unit = TimeUnit::Seconds);

    private:
        bool running = false;
        std::chrono::steady_clock::time_point start;
        std::chrono::steady_clock::time_point prevLastSignal;
        std::chrono::steady_clock::time_point lastSignal;
        std::chrono::steady_clock::time_point stop;
        const double* timeMultipliers = new double[4]
            {
                1e-9,
                    1e-6,
                    1e-3,
                    1.0,
            };
    };
}