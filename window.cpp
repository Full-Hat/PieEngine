module;
#include <string>
#include <stdexcept>
#include <memory>
#include <GLFW/glfw3.h>
#include <iostream>
#include <format>

module window;

using namespace pie_engine;

// Global GLFW initialization state
static bool g_glfw_initialized = false; // Start as false, will be set based on manual flag

GLFWwindow* create_window(int width, int height, std::string title) {
    // Initialize GLFW only once
    if (!g_glfw_initialized) {
        logger->info("Initializing GLFW...");
        if (is_manual_glfw_init) {
            // User should have called glfwInit() manually, but let's check
            if (!glfwInit()) {
                logger->error("Manual GLFW initialization requested but glfwInit() failed or was not called");
                throw std::runtime_error("Manual GLFW initialization requested but glfwInit() failed or was not called");
            }
        } else {
            // Auto-initialize GLFW
            if (!glfwInit()) {
                logger->error("Failed to initialize GLFW");
                throw std::runtime_error("Failed to initialize GLFW");
            }
        }
        g_glfw_initialized = true;
        logger->info("GLFW initialized successfully");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        logger->error(std::format("Failed to create GLFW window: {}x{} with title '{}'", width, height, title));
        throw std::runtime_error("Failed to create GLFW window");
    }

    logger->info(std::format("Created GLFW window: {}x{} with title '{}'", width, height, title));
    return window;
}

GlfwWindow::GlfwWindow() {
    logger->debug("Creating default GLFW window");
    m_window = create_window(m_width, m_height, m_title);
    m_initialized = true;
    ++s_window_count;
    logger->info(std::format("Window created. Total windows: {}", s_window_count.load()));
}

GlfwWindow::GlfwWindow(int width, int height, std::string title) {
    logger->debug(std::format("Creating GLFW window: {}x{} with title '{}'", width, height, title));
    m_width = width;
    m_height = height;
    m_title = title;
    m_window = create_window(m_width, m_height, m_title);
    m_initialized = true;
    ++s_window_count;
    logger->info(std::format("Window created. Total windows: {}", s_window_count.load()));
}

GlfwWindow::~GlfwWindow() {
    if (m_window) {
        logger->debug(std::format("Destroying GLFW window: '{}'", m_title));
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    // Only decrement count if this window was actually initialized
    if (m_initialized) {
        --s_window_count;
        logger->info(std::format("Window destroyed. Remaining windows: {}", s_window_count.load()));
        if (s_window_count == 0 && !is_manual_glfw_terminate) {
            logger->info("No more windows, terminating GLFW");
            terminate_glfw();
        }
    }
}

void GlfwWindow::PollEvents() {
    if (!m_is_open) {
        // Object can be still alive, but window is closed, so we can't actually poll events
        throw exceptions::WindowClosedException();
    }
    if (glfwWindowShouldClose(m_window)) {
        FireCloseEvent();
    }
    glfwPollEvents();
}

void GlfwWindow::FireCloseEvent() {
    if (m_is_open) {
        logger->info(std::format("Window close event fired for: '{}'", m_title));
        m_is_open = false;
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
    throw exceptions::WindowClosedException();
}

bool GlfwWindow::IsOpen() const noexcept {
    return m_is_open;
}

// Add a function to properly terminate GLFW when all windows are done
void terminate_glfw() {
    if (g_glfw_initialized && !is_manual_glfw_terminate) {
        logger->info("Terminating GLFW");
        glfwTerminate();
        g_glfw_initialized = false;
        logger->info("GLFW terminated successfully");
    }
}
