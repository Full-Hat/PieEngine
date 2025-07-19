import window.test;
import test_utils;

#include <format>

int main() {
    using namespace pie_engine::test_utils;
    using namespace pie_engine::test;

    try {
        logger.info("Starting tests...");
        // Run all window tests
        WindowTest::run_all_tests(logger);
        
        logger.info("🎉 All tests completed successfully!");
        return 0;
    }
    catch (const std::exception& e) {
        logger.error(std::format("❌ Test failed with exception: {}", e.what()));
        return 1;
    }
    catch (...) {
        logger.error("❌ Test failed with unknown exception");
        return 1;
    }
} 