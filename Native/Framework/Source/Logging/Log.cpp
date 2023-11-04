#include "Log.h"
#include "spdlog.h"

namespace Odyssey::Framework
{
	void Log::Info(const char* message)
	{
		spdlog::info(message);
	}
	void Log::Warning(const char* message)
	{
		spdlog::warn(message);
	}
	void Log::Error(const char* message)
	{
		spdlog::error(message);
	}
}