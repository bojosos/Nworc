#pragma once

#include "Crowny/Common/Common.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Crowny
{
	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		static Ref<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
	private:
		static Ref<spdlog::logger> s_ClientLogger;
		static Ref<spdlog::logger> s_EngineLogger;
	};
}

#define CW_ENGINE_INFO(...)      ::Crowny::Log::GetEngineLogger()->info(__VA_ARGS__)
#define CW_ENGINE_WARN(...)      ::Crowny::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define CW_ENGINE_ERROR(...)     ::Crowny::Log::GetEngineLogger()->error(__VA_ARGS__)
#define CW_ENGINE_CRITICAL(...)  ::Crowny::Log::GetEngineLogger()->critical(__VA_ARGS__)

#define CW_INFO(...)             ::Crowny::Log::GetClientLogger()->info(__VA_ARGS__)
#define CW_WARN(...)             ::Crowny::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CW_ERROR(...)            ::Crowny::Log::GetClientLogger()->error(__VA_ARGS__)
#define CW_CRITICAL(...)         ::Crowny::Log::GetClientLogger()->critical(__VA_ARGS__)