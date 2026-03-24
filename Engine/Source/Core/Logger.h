#pragma once

#include <memory>
#include <string>
#include <spdlog/spdlog.h>

namespace Nova {

    class Logger {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger>& GetEngineLogger()  { return s_EngineLogger; }
        static std::shared_ptr<spdlog::logger>& GetClientLogger()  { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_EngineLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };

} // namespace Nova

// ── Engine Logging Macros ───────────────────────────────────────────────────
#define NOVA_ENGINE_TRACE(...)   ::Nova::Logger::GetEngineLogger()->trace(__VA_ARGS__)
#define NOVA_ENGINE_INFO(...)    ::Nova::Logger::GetEngineLogger()->info(__VA_ARGS__)
#define NOVA_ENGINE_WARN(...)    ::Nova::Logger::GetEngineLogger()->warn(__VA_ARGS__)
#define NOVA_ENGINE_ERROR(...)   ::Nova::Logger::GetEngineLogger()->error(__VA_ARGS__)

// ── Client Logging Macros ───────────────────────────────────────────────────
#define NOVA_LOG_TRACE(...)      ::Nova::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define NOVA_LOG_INFO(...)       ::Nova::Logger::GetClientLogger()->info(__VA_ARGS__)
#define NOVA_LOG_WARN(...)       ::Nova::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define NOVA_LOG_ERROR(...)      ::Nova::Logger::GetClientLogger()->error(__VA_ARGS__)
