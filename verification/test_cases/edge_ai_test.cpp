#include <gtest/gtest.h>
#include "../verification_environment.h"
#include "test_case.h"

class EdgeAITestCase : public ::testing::Test {
protected:
    ShaderVerificationEnv* env;
    EdgeAIInferenceTest* test;
    
    void SetUp() override {
        // Set up the verification environment with proper configuration
        env = new ShaderVerificationEnv("verification_env");
        
        // Create test case
        test = new EdgeAIInferenceTest("edge_ai_inference_test");
        
        // Configure environment
        VerificationConfig config;
        config.max_cycles = 500;
        config.trace_enabled = true;
        config.trace_file = "edge_ai_test.vcd";
        config.verbose_logging = true;
        
        env->configure(config);
        
        // Set up test case with verification environment components
        test->set_dut(env->dut);
        test->set_reference(env->ref_model);
        test->set_scoreboard(env->scoreboard);
        test->set_coverage(env->coverage);
        // Config is handled by the run_test method
    }
    
    void TearDown() override {
        // Clean up resources
        delete test;
        delete env;
    }
};

TEST_F(EdgeAITestCase, InferenceOptimization) {
    // Run the test using the verification environment
    bool result = env->run_test(test);
    
    // Check result
    EXPECT_TRUE(result) << "Edge AI test failed";
    
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