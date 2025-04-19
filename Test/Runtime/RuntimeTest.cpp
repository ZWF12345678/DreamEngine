#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"


#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>

int main()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/mixed.log", true);

    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

    auto mixed_logger = std::make_shared<spdlog::logger>("mixed", begin(sinks), end(sinks));
    spdlog::register_logger(mixed_logger);
    mixed_logger->info("Logging to both console and file.");

    SPDLOG_LOGGER_INFO(mixed_logger, "Support for floats {}", "hello world");


    std::getchar();

    spdlog::shutdown();
    
    return 0;
}