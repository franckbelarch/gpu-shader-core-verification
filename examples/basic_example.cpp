#include <iostream>
#include <memory>
#include "../verification/test_cases/test_case.h"
#include "../model/shader_core/shader_core.h"
#include "../verification/checkers/scoreboard.h"

/**
 * Basic example showing how to set up and run a simple verification test
 * for the GPU shader core.
 */
int main() {
    std::cout << "Setting up basic GPU shader core verification example..." << std::endl;
    
    // Initialize the DUT (Device Under Test)
    std::unique_ptr<ShaderCore> shader_core = std::make_unique<ShaderCore>();
    
    // Initialize verification components
    std::unique_ptr<Scoreboard> scoreboard = std::make_unique<Scoreboard>();
    
    // Create and configure test case
    std::unique_ptr<TestCase> test_case = std::make_unique<TestCase>("Basic Example");
    test_case->configure(shader_core.get(), scoreboard.get());
    
    // Run the test
    std::cout << "Running basic test..." << std::endl;
    bool result = test_case->run();
    
    // Check results
    if (result) {
        std::cout << "Test PASSED!" << std::endl;
    } else {
        std::cout << "Test FAILED!" << std::endl;
    }
    
    // Print coverage information
    test_case->print_coverage();
    
    return result ? 0 : 1;
}