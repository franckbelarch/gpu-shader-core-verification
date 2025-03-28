#include <gtest/gtest.h>
#include "../verification_environment.h"
#include "test_case.h"

class BasicTensorTestCase : public ::testing::Test {
protected:
    TensorCoreTest* test;
    Scoreboard* scoreboard;
    
    void SetUp() override {
        std::cout << "SetUp - Starting" << std::endl;
        
        // Create components with minimal connections
        test = new TensorCoreTest("basic_tensor_test");
        scoreboard = new Scoreboard();
        test->set_scoreboard(scoreboard);
        
        std::cout << "SetUp - Completed" << std::endl;
    }
    
    void TearDown() override {
        std::cout << "TearDown - Starting" << std::endl;
        
        // Set flag so sc_main can clean up
        done = true;
        
        // Delete resources in reverse order
        delete test;
        delete scoreboard;
        
        std::cout << "TearDown - Completed" << std::endl;
    }
    
    // Static flag to indicate test completion - must be public for sc_main to access
public:
    static bool done;
};

TEST_F(BasicTensorTestCase, MatrixMultiply) {
    std::cout << "TestBody - Starting" << std::endl;
    
    // Run the basic test phases
    test->setup();
    test->execute();
    bool result = test->verify_results();
    test->cleanup();
    
    // Verify the results 
    EXPECT_TRUE(result);
    
    // Get the log and metrics
    const TestCase::TestResult& test_results = test->get_results();
    
    // Print the results
    std::cout << "Log messages:" << std::endl;
    for (const auto& msg : test->get_log()) {
        std::cout << "  " << msg << std::endl;
    }
    
    std::cout << "Metrics:" << std::endl;
    for (const auto& metric : test_results.metrics) {
        std::cout << "  " << metric.first << ": " << metric.second << std::endl;
    }
    
    std::cout << "TestBody - Completed" << std::endl;
}

// Add a direct tensor unit test that doesn't require SystemC
TEST_F(BasicTensorTestCase, DirectTensorTest) {
    std::cout << "DirectTensorTest - Starting" << std::endl;
    
    // Create tensors for multiplication (2x2 matrices)
    std::vector<size_t> dims_a = {2, 2};
    TensorData tensor_a(dims_a, TensorData::Precision::FP32);  // Use FP32 to avoid precision issues
    tensor_a.set_fp32(0, 1.0f);  // [0,0] = 1.0
    tensor_a.set_fp32(1, 2.0f);  // [0,1] = 2.0
    tensor_a.set_fp32(2, 3.0f);  // [1,0] = 3.0
    tensor_a.set_fp32(3, 4.0f);  // [1,1] = 4.0
    
    std::vector<size_t> dims_b = {2, 2};
    TensorData tensor_b(dims_b, TensorData::Precision::FP32);  // Use FP32 to avoid precision issues
    tensor_b.set_fp32(0, 5.0f);  // [0,0] = 5.0
    tensor_b.set_fp32(1, 6.0f);  // [0,1] = 6.0
    tensor_b.set_fp32(2, 7.0f);  // [1,0] = 7.0
    tensor_b.set_fp32(3, 8.0f);  // [1,1] = 8.0
    
    // Print input matrices with actual values we read back
    std::cout << "Matrix A (actual values read back):" << std::endl;
    for (size_t i = 0; i < 2; i++) {
        std::cout << "  ";
        for (size_t j = 0; j < 2; j++) {
            std::cout << tensor_a.get_fp32(i*2+j) << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Matrix B (actual values read back):" << std::endl;
    for (size_t i = 0; i < 2; i++) {
        std::cout << "  ";
        for (size_t j = 0; j < 2; j++) {
            std::cout << tensor_b.get_fp32(i*2+j) << " ";
        }
        std::cout << std::endl;
    }
    
    // Calculate result manually
    std::vector<size_t> dims_result = {2, 2};
    TensorData manual_result(dims_result, TensorData::Precision::FP32);
    
    // Manually perform matrix multiplication
    // C[0,0] = A[0,0]*B[0,0] + A[0,1]*B[1,0]
    manual_result.set_fp32(0, 
        tensor_a.get_fp32(0) * tensor_b.get_fp32(0) + 
        tensor_a.get_fp32(1) * tensor_b.get_fp32(2));
    
    // C[0,1] = A[0,0]*B[0,1] + A[0,1]*B[1,1]
    manual_result.set_fp32(1, 
        tensor_a.get_fp32(0) * tensor_b.get_fp32(1) + 
        tensor_a.get_fp32(1) * tensor_b.get_fp32(3));
    
    // C[1,0] = A[1,0]*B[0,0] + A[1,1]*B[1,0]
    manual_result.set_fp32(2, 
        tensor_a.get_fp32(2) * tensor_b.get_fp32(0) + 
        tensor_a.get_fp32(3) * tensor_b.get_fp32(2));
    
    // C[1,1] = A[1,0]*B[0,1] + A[1,1]*B[1,1]
    manual_result.set_fp32(3, 
        tensor_a.get_fp32(2) * tensor_b.get_fp32(1) + 
        tensor_a.get_fp32(3) * tensor_b.get_fp32(3));
    
    // Print result
    std::cout << "Manual Matrix Multiplication Result:" << std::endl;
    for (size_t i = 0; i < 2; i++) {
        std::cout << "  ";
        for (size_t j = 0; j < 2; j++) {
            std::cout << manual_result.get_fp32(i*2+j) << " ";
        }
        std::cout << std::endl;
    }
    
    // Verify our manually calculated values
    EXPECT_FLOAT_EQ(manual_result.get_fp32(0), 19.0f);
    EXPECT_FLOAT_EQ(manual_result.get_fp32(1), 22.0f);
    EXPECT_FLOAT_EQ(manual_result.get_fp32(2), 43.0f);
    EXPECT_FLOAT_EQ(manual_result.get_fp32(3), 50.0f);
    
    std::cout << "DirectTensorTest - Completed Successfully" << std::endl;
}

// Initialize the static flag
bool BasicTensorTestCase::done = false;

// SystemC main function with GoogleTest integration
// This is the required entry point for SystemC applications
int sc_main(int argc, char **argv) {
    std::cout << "Starting SystemC test harness" << std::endl;
    
    // Set SystemC time resolution globally
    sc_set_time_resolution(1, SC_NS);
    sc_set_default_time_unit(1, SC_NS);
    
    // Initialize GoogleTest
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run the tests
    int result = RUN_ALL_TESTS();
    
    // Ensure tests are complete and properly stopped
    if (!BasicTensorTestCase::done) {
        std::cout << "Warning: Tests did not complete properly" << std::endl;
    }
    
    // Force stop any remaining SystemC process
    sc_stop();
    
    std::cout << "Tests complete, final time: " << sc_time_stamp() << std::endl;
    
    // Return test result
    return result;
}