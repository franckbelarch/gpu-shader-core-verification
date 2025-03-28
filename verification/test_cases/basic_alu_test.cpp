#include <gtest/gtest.h>
#include "../verification_environment.h"
#include "test_case.h"

class BasicALUTestCase : public ::testing::Test {
protected:
    ShaderVerificationEnv* env;
    ShaderALUTest* test;
    
    void SetUp() override {
        // Skip setting up verification environment for now
        env = nullptr;
        
        // Create test case
        test = new ShaderALUTest("basic_alu_test");
        
        // Configure test directly without environment
        test->config = nullptr;
        test->set_scoreboard(new Scoreboard());
    }
    
    void TearDown() override {
        // In this simplified approach, the Scoreboard is owned by the TestCase
        // and will be deleted in the TestCase destructor
        delete test;
    }
};

TEST_F(BasicALUTestCase, ALUOperations) {
    // Run the test using the verification environment
    bool result = env->run_test(test);
    
    // Check result
    EXPECT_TRUE(result) << "ALU test failed";
    
    // Check that the test passed
    EXPECT_TRUE(test->passed()) << "Test not marked as passed";
    
    // Get test results
    const TestCase::TestResult& results = test->get_results();
    
    // Check that we have some metrics
    EXPECT_FALSE(results.metrics.empty()) << "No metrics collected";
}

// SystemC main function with GoogleTest integration
// This is the required entry point for SystemC applications
int sc_main(int argc, char **argv) {
    // Initialize GoogleTest
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run the tests
    int result = RUN_ALL_TESTS();
    
    // Return test result
    return result;
}