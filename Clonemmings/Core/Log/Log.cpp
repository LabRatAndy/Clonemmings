#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <vector>
namespace Clonemmings
{
	std::shared_ptr<spdlog::logger> Log::s_Logger;

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> logsinks;
		logsinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logsinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Clonemmings.log", true));

		logsinks[0]->set_pattern("%^[%T] %n: %v%$");
		logsinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_Logger = std::make_shared<spdlog::logger>("Clonemmings", begin(logsinks), end(logsinks));
		spdlog::register_logger(s_Logger);
		s_Logger->set_level(spdlog::level::trace);
		s_Logger->flush_on(spdlog::level::trace);
	}
}