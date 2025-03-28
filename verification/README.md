# GPU Shader Core Verification

This directory contains the verification infrastructure for the GPU shader core model.

## Structure

- `test_cases/`: Implementation of various test cases
- `checkers/`: Result checking components (scoreboards)
- `monitors/`: Activity monitors for interfaces
- `coverage/`: Coverage collection components
- `generators/`: Random stimulus generators

## Test Categories

1. **Basic ALU Tests**: Verify arithmetic, logical, and shift operations
2. **Memory Tests**: Test memory transactions across the hierarchy
3. **Tensor Tests**: Validate tensor operations with different precisions
4. **Edge AI Tests**: Test inference optimizations for edge AI

## Running Tests

Use the provided script to run all tests:

```
cd build
./run_all_tests.sh
```

Results are stored in the `test-results/` directory and summarized in `test_summary.html`.