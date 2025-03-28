# GPU Shader Core Model

This directory contains the SystemC reference model for the GPU shader core with specialized tensor acceleration capabilities.

## Structure

- `shader_core/`: The main shader core implementation with instruction pipeline
- `tensor_unit/`: Tensor processing unit for matrix/vector operations
- `memory_subsystem/`: Hierarchical memory model with register files, caches, and HBM3e

## Features

- Full instruction set support (ALU, tensor, memory, control flow)
- Multi-precision support (FP32, FP16, FP8, FP4)
- Edge AI optimization capabilities
- Configurable memory hierarchy

## Building

The model is built using CMake:

```
mkdir build
cd build
cmake ..
make
```

See the main README for more detailed build instructions.