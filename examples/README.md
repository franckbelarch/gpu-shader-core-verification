# GPU Shader Core Verification Examples

This directory contains examples demonstrating how to use the GPU shader core verification framework.

## Setting Up and Running Examples

All examples can be built and run with the following commands:

```bash
mkdir build
cd build
cmake ..
make
./verification/example_name
```

## Basic Examples

These examples demonstrate fundamental verification techniques:

1. **Basic ALU Test**: Simple test of arithmetic operations
2. **Memory Test**: Testing memory subsystem functionality
3. **Tensor Test**: Validation of tensor core operations
4. **Edge AI Test**: Testing specialized edge AI capabilities

## Advanced Examples

These examples show more complex verification scenarios:

1. **Coverage-Driven Test**: Example of coverage-driven test methodology
2. **AI-Assisted Test Generation**: Using the AI tools for test optimization
3. **Performance Verification**: Measuring and validating performance metrics

## Example Structure

Each example follows a similar structure:

1. Setup the verification environment
2. Configure the test parameters
3. Run the test scenarios
4. Check results and coverage
5. Generate reports

Refer to the documentation in `docs/` for more details on the verification methodology.