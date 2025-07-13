import window.test;

#include <iostream>

int main() {
    try {
        // Run all window tests
        pie_engine::test::WindowTest::run_all_tests();
        
        std::cout << "\n🎉 All tests completed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
} 