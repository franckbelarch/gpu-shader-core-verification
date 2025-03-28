#ifndef TENSOR_OPCODE_H
#define TENSOR_OPCODE_H

#include <systemc.h>

// Opcodes for tensor operations
enum class TensorOpcode {
    // Matrix operations
    MATRIX_MULTIPLY_FP16,
    MATRIX_MULTIPLY_FP8,
    MATRIX_MULTIPLY_FP4,
    MATRIX_TRANSPOSE,
    
    // Vector operations
    VECTOR_DOT_PRODUCT,
    VECTOR_ADD,
    VECTOR_SUB,
    VECTOR_MUL,
    VECTOR_DIV,
    
    // Convolution operations
    CONV_2D,
    CONV_3D,
    DEPTHWISE_CONV,
    
    // Transformer operations
    ATTENTION,
    LAYER_NORM,
    SOFTMAX,
    
    // Activation functions
    RELU,
    GELU,
    SIGMOID,
    TANH,
    
    // Special operations
    TENSOR_COPY,
    TENSOR_FILL,
    TENSOR_TRANSPOSE,
    TENSOR_RESHAPE,
    
    // No operation
    NOP
};

// Comparison operators for enums are already provided by the compiler

// SystemC tracing for TensorOpcode
namespace sc_core {
    // Forward declarations for external implementation
    void sc_trace(sc_trace_file* tf, const TensorOpcode& op, const std::string& name);
    std::ostream& operator<<(std::ostream& os, const TensorOpcode& op);
}

#endif // TENSOR_OPCODE_H