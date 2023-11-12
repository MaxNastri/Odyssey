#include "Logger.h"
#include "spdlog.h"

namespace Odyssey::Framework
{
	void Logger::LogInfo(const char* message)
	{
		spdlog::info(message);
	}

	void Logger::LogInfo(std::string message)
	{
		spdlog::info(message);
	}

	void Logger::LogInfo(std::string_view message)
	{
		spdlog::info(message);
	}

	void Logger::LogWarning(const char* message)
	{
		spdlog::warn(message);
	}

	void Logger::LogError(const char* message)
	{
		spdlog::error(message);
	}

	void Logger::LogError(std::string message)
	{
		spdlog::error(message);
	}

	void Logger::LogError(std::string_view message)
	{
		spdlog::error(message);
	}
}