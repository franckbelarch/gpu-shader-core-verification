#ifndef TENSOR_UNIT_H
#define TENSOR_UNIT_H

#include <systemc.h>
#include "tensor_data.h"
#include "tensor_buffer.h"
#include "tensor_opcode.h"

class TensorUnit : public sc_module {
public:
    // Ports
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<TensorOpcode> opcode;
    sc_in<TensorData> input_a;
    sc_in<TensorData> input_b;
    sc_out<TensorData> output;
    
    // Constructor and destructor
    SC_HAS_PROCESS(TensorUnit);
    TensorUnit(sc_module_name name);
    ~TensorUnit();
    
    // Processes
    void execute_tensor_op();
    
    // SystemC callbacks
    void before_end_of_elaboration() override;
    
    // Precision-specific operations
    void matrix_multiply_fp16();
    void matrix_multiply_fp8();
    void matrix_multiply_fp4();
    void vector_dot_product();
    void convolution_2d();
    
    // Transformer-specific operations
    void attention_mechanism();
    void layer_normalization();
    
private:
    // Internal state and buffers
    TensorBuffer buffer_a;
    TensorBuffer buffer_b;
    TensorBuffer result_buffer;
    
    // Internal signals for self-binding ports to avoid port binding errors
    sc_signal<TensorOpcode>* internal_opcode_signal;
    sc_signal<TensorData>* internal_input_a_signal;
    sc_signal<TensorData>* internal_input_b_signal;
    sc_signal<TensorData>* internal_output_signal;
};

#endif // TENSOR_UNIT_H