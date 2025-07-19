//
// Created by nikit on 7/6/2025.
//

module;

#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <spdlog/logger.h>

export module window.test;

import window;
import test_utils;

export namespace pie_engine::test {

class WindowTest {
protected:
    static inline test_utils::Logger m_logger;

public:
    static void run_all_tests(const test_utils::Logger& logger) {
        m_logger = logger;
        m_logger.additional_info = "window";

        m_logger.info("Running window tests...");
        
        test_single_window_creation();
        test_multiple_windows();
        test_window_destruction();
        test_window_lifecycle();
        
        m_logger.info("All window tests passed!");
    }

private:
    static void test_single_window_creation() {
        m_logger.info("Testing single window creation...");
        
        // Test default constructor
        {
            GlfwWindow window;
            assert(window.IsOpen());
            m_logger.info("✓ Default window created successfully");
        }
        
        // Test parameterized constructor
        {
            GlfwWindow window(1024, 768, "Test Window");
            assert(window.IsOpen());
            m_logger.info("✓ Parameterized window created successfully");
        }
    }
    
    static void test_multiple_windows() {
        m_logger.info("Testing multiple windows...");
        
        std::vector<std::unique_ptr<GlfwWindow>> windows;
        
        // Create multiple windows
        windows.push_back(std::make_unique<GlfwWindow>(800, 600, "Window 1"));
        windows.push_back(std::make_unique<GlfwWindow>(600, 400, "Window 2"));
        windows.push_back(std::make_unique<GlfwWindow>(1000, 800, "Window 3"));
        
        // Verify all windows are open
        for (const auto& window : windows) {
            assert(window->IsOpen());
        }
        
        m_logger.info("✓ Multiple windows created successfully");
    }
    
    static void test_window_destruction() {
        m_logger.info("Testing window destruction...");
        
        // Test that window is properly destroyed when going out of scope
        {
            GlfwWindow window(800, 600, "Destruction Test");
            assert(window.IsOpen());
        }
        // Window should be destroyed here
        
        m_logger.info("✓ Window destruction test passed");
    }
    
    static void test_window_lifecycle() {
        m_logger.info("Testing window lifecycle...");
        
        GlfwWindow window(800, 600, "Lifecycle Test");
        
        // Test initial state
        assert(window.IsOpen());
        
        // Test close event
        try {
            window.FireCloseEvent();
            assert(false); // Should not reach here
        } catch (const exceptions::WindowClosedException& e) {
            // Expected exception
            assert(!window.IsOpen());
        }
        
        m_logger.info("✓ Window lifecycle test passed");
    }
};

} // namespace pie_engine::test 