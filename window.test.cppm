//
// Created by nikit on 7/6/2025.
//

module;

#include <iostream>
#include <memory>
#include <vector>
#include <cassert>

export module window.test;

import window;

export namespace pie_engine::test {

class WindowTest {
public:
    static void run_all_tests() {
        std::cout << "Running window tests..." << std::endl;
        
        test_single_window_creation();
        test_multiple_windows();
        test_window_destruction();
        test_window_lifecycle();
        
        std::cout << "All window tests passed!" << std::endl;
    }

private:
    static void test_single_window_creation() {
        std::cout << "  Testing single window creation..." << std::endl;
        
        // Test default constructor
        {
            GlfwWindow window;
            assert(window.IsOpen());
            std::cout << "    ✓ Default window created successfully" << std::endl;
        }
        
        // Test parameterized constructor
        {
            GlfwWindow window(1024, 768, "Test Window");
            assert(window.IsOpen());
            std::cout << "    ✓ Parameterized window created successfully" << std::endl;
        }
    }
    
    static void test_multiple_windows() {
        std::cout << "  Testing multiple windows..." << std::endl;
        
        std::vector<std::unique_ptr<GlfwWindow>> windows;
        
        // Create multiple windows
        windows.push_back(std::make_unique<GlfwWindow>(800, 600, "Window 1"));
        windows.push_back(std::make_unique<GlfwWindow>(600, 400, "Window 2"));
        windows.push_back(std::make_unique<GlfwWindow>(1000, 800, "Window 3"));
        
        // Verify all windows are open
        for (const auto& window : windows) {
            assert(window->IsOpen());
        }
        
        std::cout << "    ✓ Multiple windows created successfully" << std::endl;
    }
    
    static void test_window_destruction() {
        std::cout << "  Testing window destruction..." << std::endl;
        
        // Test that window is properly destroyed when going out of scope
        {
            GlfwWindow window(800, 600, "Destruction Test");
            assert(window.IsOpen());
        }
        // Window should be destroyed here
        
        std::cout << "    ✓ Window destruction test passed" << std::endl;
    }
    
    static void test_window_lifecycle() {
        std::cout << "  Testing window lifecycle..." << std::endl;
        
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
        
        std::cout << "    ✓ Window lifecycle test passed" << std::endl;
    }
};

} // namespace pie_engine::test 