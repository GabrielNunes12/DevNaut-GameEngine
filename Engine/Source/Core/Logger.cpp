#include "Core/Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Nova {

    std::shared_ptr<spdlog::logger> Logger::s_EngineLogger;
    std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

    void Logger::Init() {
        // Pattern: [timestamp] [logger name] message
        spdlog::set_pattern("%^[%T] [%n] %v%$");

        s_EngineLogger = spdlog::stdout_color_mt("NOVA");
        s_EngineLogger->set_level(spdlog::level::trace);

        s_ClientLogger = spdlog::stdout_color_mt("APP");
        s_ClientLogger->set_level(spdlog::level::trace);

        s_EngineLogger->info("Logger initialized");
    }

} // namespace Nova
