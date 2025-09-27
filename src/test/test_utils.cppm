module;

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <string>
#include <format>

export module test_utils;

export namespace pie_engine::test_utils {
struct Logger {
    using inner_logger_t = std::shared_ptr<spdlog::logger>;
    inner_logger_t logger;
    std::string additional_info = "global";

    #define LOG_FUNCTION_DEFINITION(level) \
        template <typename T> \
        void level(const T &msg) { \
            if (additional_info.empty()) { \
                logger->level(msg); \
            } else { \
                logger->level(std::format("[{}] {}", additional_info, msg)); \
            } \
        }

    LOG_FUNCTION_DEFINITION(trace);
    LOG_FUNCTION_DEFINITION(debug);
    LOG_FUNCTION_DEFINITION(info);
    LOG_FUNCTION_DEFINITION(warn);
    LOG_FUNCTION_DEFINITION(error);
    LOG_FUNCTION_DEFINITION(critical);
};

Logger logger = [](){
    const auto logger = spdlog::basic_logger_mt("pie_engine::tests", "logs/tests.txt");
    // print all, flush on all
    logger->flush_on(spdlog::level::trace);
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [thread %t] %v");
    return Logger{
        .logger = logger
    };
}();
}