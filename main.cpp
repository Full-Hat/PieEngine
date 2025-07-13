import window;

#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <algorithm>

int main() {
    std::vector<std::unique_ptr<pie_engine::GlfwWindow>> windows;
    
    try {
        // Create multiple windows
        windows.push_back(std::make_unique<pie_engine::GlfwWindow>(800, 600, "Window 1"));
        windows.push_back(std::make_unique<pie_engine::GlfwWindow>(600, 400, "Window 2"));
        windows.push_back(std::make_unique<pie_engine::GlfwWindow>(1000, 800, "Window 3"));
        
        std::cout << "Created " << windows.size() << " windows. Close any window to exit." << std::endl;
        
        // Main loop - poll events for all windows
        while (!windows.empty()) {
            // Poll events for all windows
            for (auto& window : windows) {
                try {
                    window->PollEvents();
                } catch (const pie_engine::exceptions::WindowClosedException& e) {
                    std::cout << "Window closed: " << e.what() << std::endl;
                    // Remove closed windows from the list
                    windows.erase(std::remove_if(windows.begin(), windows.end(),
                        [](const auto& w) { return !w->IsOpen(); }), windows.end());
                    break; // Exit the for loop to avoid iterator invalidation
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}