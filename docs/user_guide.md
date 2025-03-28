# GPU Shader Core Verification Framework User Guide

## Introduction

This guide explains how to use the GPU Shader Core Verification Framework, including running tests, analyzing coverage, and using the AI-assisted verification tools.

## Setting Up the Environment

### Installing Dependencies

The verification framework requires the following dependencies:

- SystemC 2.3.3+
- GCC 7+ or Clang 6+ (with C++17 support)
- Python 3.8+
- CMake 3.10+
- Python packages: numpy, scikit-learn, matplotlib, etc.

You can install Python dependencies using the provided requirements file:

```bash
pip install -r infrastructure/requirements.txt
```

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/gpu-shader-core-v.git
   cd gpu-shader-core-v
   ```

2. Create a build directory and configure the project:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   ```

3. Build the project:
   ```bash
   make
   ```

### Using Docker

Alternatively, you can use the provided Docker environment:

```bash
docker build -t gpu-shader-verification -f infrastructure/docker/Dockerfile .
docker run -it gpu-shader-verification
```

## Running Tests

### Running All Tests

To run all available tests:

```bash
./build/run_all_tests.sh
```

### Running Specific Test Categories

To run specific test categories:

```bash
# Basic ALU tests
./build/verification/basic_alu_tests

# Tensor core tests
./build/verification/basic_tensor_tests

# Memory subsystem tests
./build/verification/basic_memory_tests

# Edge AI tests
./build/verification/edge_ai_tests
```

### Advanced and Regression Tests

```bash
# Advanced tests
./infrastructure/scripts/run_advanced_tests.sh

# Regression tests
./infrastructure/scripts/run_regression_tests.sh
```

## Analyzing Test Results

### Test Summary

After running tests, a summary report is generated that provides an overview of test results:

```bash
# Generate test summary
python infrastructure/scripts/generate_test_summary.py build/test-results build/test_summary.html
```

Open the generated HTML file in a browser to view the results.

### Coverage Analysis

To analyze code coverage after running tests:

```bash
# Collect coverage data
./infrastructure/scripts/collect_coverage.sh

# View coverage report
open build/coverage/html/index.html
```

## Using AI-Assisted Verification Tools

### Test Optimization

The test optimization tool uses machine learning to suggest effective test cases based on current coverage:

```bash
python ai_tools/test_optimization/test_optimizer.py data/coverage_data.json
```

Interactive commands within the tool:
- `1`: Train model based on previous test results
- `2`: Suggest next test to run
- `3`: Identify coverage gaps
- `4`: Add new test result
- `5`: Save data
- `6`: Exit

### Coverage Gap Analysis

The coverage gap analyzer helps identify areas that need more testing:

```bash
python ai_tools/coverage_analysis/coverage_gap_analyzer.py data/coverage_data.json
```

Interactive commands within the tool:
- `1`: Identify coverage gaps
- `2`: Get coverage summary
- `3`: Analyze coverage trend
- `4`: Suggest tests for gaps
- `5`: Generate coverage plot
- `6`: Analyze test effectiveness
- `7`: Exit

To generate a coverage plot automatically:

```bash
python ai_tools/coverage_analysis/coverage_gap_analyzer.py data/coverage_data.json --generate-plot build/coverage/trend.png
```

## Extending the Framework

### Adding New Test Cases

To add a new test case:

1. Create a new C++ file in `verification/test_cases/`
2. Inherit from the `TestCase` base class
3. Implement the `setup()`, `execute()`, and `cleanup()` methods
4. Add the test case to the `verification/CMakeLists.txt` file

Example:

```cpp
// verification/test_cases/my_custom_test.cpp
#include "test_case.h"

class MyCustomTest : public TestCase {
public:
    MyCustomTest(const std::string& test_name) : TestCase(test_name) {}
    virtual ~MyCustomTest() {}
    
    virtual void setup() override {
        // Test setup code
    }
    
    virtual void execute() override {
        // Test execution code
    }
    
    virtual void cleanup() override {
        // Test cleanup code
    }
};
```

### Adding New Coverage Metrics

To add new coverage metrics:

1. Create a new class that inherits from `CoverageDomain` in `verification/coverage/`
2. Implement the required methods
3. Add the new domain to `CoverageCollector` constructor

### Integrating with CI/CD

The framework includes Jenkins pipeline configuration for CI/CD integration:

```bash
# Run Jenkins pipeline
jenkins-jobs update infrastructure/jenkins/Jenkinsfile
```

## Troubleshooting

### Common Issues

1. **SystemC not found**: Make sure SystemC is installed and set the `SYSTEMC_HOME` environment variable.

2. **Tests fail with segmentation fault**: This is often due to memory issues or incorrect pointers.

3. **Coverage collection fails**: Ensure code is compiled with coverage flags (`-DENABLE_COVERAGE=ON`).

4. **AI tools crash**: Check Python dependencies and ensure the `data/coverage_data.json` file exists.

### Getting Help

If you encounter issues, you can:

1. Check the logs in `build/logs/`
2. Run tests with verbose logging enabled:
   ```bash
   VERBOSE=1 ./build/run_all_tests.sh
   ```
3. File an issue on the GitHub repository

## Reference Information

### Project Structure

- `model/`: SystemC reference model
- `verification/`: Verification infrastructure
- `infrastructure/`: Build and CI/CD scripts
- `ai_tools/`: AI-assisted verification tools
- `docs/`: Documentation

### Environment Variables

- `SYSTEMC_HOME`: Path to SystemC installation
- `VERBOSE`: Enable verbose logging (0 or 1)
- `COVERAGE`: Enable coverage collection (0 or 1)