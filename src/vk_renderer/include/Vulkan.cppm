//
// Created by FullHat on 22/07/2025.
//
module;

#include <GLFW/glfw3.h>

#include "vulkan/vulkan.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

export module Vulkan;

constexpr int_fast32_t one_megabyte = 1048576;
auto logger = [](){
    auto logger = spdlog::rotating_logger_mt("pie_engine::vk_renderer", "logs/vk_renderer_rotating.txt", one_megabyte * 5, 3);
    // Debug -> print all, flush on all
    // Release -> print info and higher, flush on error only
    logger->flush_on(spdlog::level::err);
    logger->set_level(spdlog::level::info);
#ifndef NDEBUG
    logger->flush_on(spdlog::level::trace);
    logger->set_level(spdlog::level::trace);
#endif
    return logger;
}();

export class Vulkan {
public:
    void InitInstance();

private:
    vk::Instance m_Instance;
};
