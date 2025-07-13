//
// Created by nikit on 7/6/2025.
//

module;

#include <iostream>
#include <format>
#include <cstdint>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <GLFW/glfw3.h>

export module window;

//! @section Glfw helpers

// Global configuration variables - can be overridden by defining them before importing the module
#ifndef is_manual_glfw_init
const bool is_manual_glfw_init = false; // If true, glfw will not be initialized automatically, and you need to call glfwInit() manually
#endif

#ifndef is_manual_glfw_terminate  
const bool is_manual_glfw_terminate = false; // If true, glfw will not be terminated automatically, and you need to call glfwTerminate() manually
#endif

GLFWwindow* create_window(int width, int height, std::string title);
void terminate_glfw(); // Call this when all windows are done

//! @section pie engine logging
constexpr int_fast32_t one_megabyte = 1048576;
auto logger = [](){
    auto logger = spdlog::rotating_logger_mt("pie_engine::window", "logs/rotating.txt", one_megabyte * 5, 3);
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
    

//! @section pie engine Window class

export namespace pie_engine {

namespace exceptions {
class WindowClosedException : public std::runtime_error {
public:
    WindowClosedException() : std::runtime_error("Window close event fired") {}
};
}

class GlfwWindow {
public:
    GlfwWindow();
    GlfwWindow(int width, int height, std::string title);
    ~GlfwWindow();
    
    void PollEvents(); // throws exceptions::WindowClosedException if window is closed, fetch updates
    void FireCloseEvent(); // throws exceptions::WindowClosedException

    bool IsOpen() const noexcept;
    
protected:
    int_fast16_t m_width = 800;
    int_fast16_t m_height = 600;
    std::string m_title = "";
    GLFWwindow* m_window = nullptr;

    bool m_is_open = true;
    bool m_initialized = false; // Track if window was successfully created

private:
    static inline size_t s_window_count = 0;
};
}