#ifndef SHADER_CORE_H
#define SHADER_CORE_H

#include <systemc.h>
#include "../tensor_unit/tensor_unit.h"
#include "../memory_subsystem/memory_subsystem.h"
#include "instruction_decoder.h"
#include "register_file.h"
#include "execution_unit.h"
#include "../tensor_unit/tensor_data.h"
#include "instruction_buffer.h"

class ShaderCore : public sc_module {
public:
    // Ports
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<sc_uint<32>> instruction;
    sc_in<sc_uint<32>> operand_a;
    sc_in<sc_uint<32>> operand_b;
    sc_out<sc_uint<32>> result;
    
    // Internal components
    InstructionDecoder* decoder;
    ShaderRegisterFile* reg_file;
    ExecutionUnit* exec_unit;
    TensorUnit* tensor_unit;
    MemorySubsystem* mem_subsys;
    
    // Constructor and destructor
    SC_HAS_PROCESS(ShaderCore);
    ShaderCore(sc_module_name name);
    ~ShaderCore();
    
    // Processes
    void execute();
    
    // Tensor acceleration methods
    void tensor_multiply_fp16(const TensorData& a, const TensorData& b, TensorData& result);
    void tensor_multiply_fp8(const TensorData& a, const TensorData& b, TensorData& result);
    void tensor_multiply_fp4(const TensorData& a, const TensorData& b, TensorData& result);
    
    // Edge AI optimized methods
    void inference_optimize();
    
    // Signal connection methods for testing
    void connect_tensor_ports(
        sc_signal<TensorOpcode>& opcode_sig,
        sc_signal<TensorData>& input_a_sig,
        sc_signal<TensorData>& input_b_sig,
        sc_signal<TensorData>& output_sig);
    
private:
    // Internal state
    sc_uint<32> pc;
    InstructionBuffer* instr_buffer;
    bool stall;
    
    // Internal signals for component connections
    sc_signal<sc_uint<32>>* instr_decoder_sig;
    sc_signal<DecodedInstruction>* decoded_instr_sig;
    sc_signal<sc_uint<32>>* alu_op_a_sig;
    sc_signal<sc_uint<32>>* alu_op_b_sig;
    sc_signal<sc_uint<32>>* alu_result_sig;
    sc_signal<bool>* exec_busy_sig;
    
    // Signals for tensor unit
    sc_signal<TensorOpcode>* tensor_opcode_sig;
    sc_signal<TensorData>* tensor_input_a_sig;
    sc_signal<TensorData>* tensor_input_b_sig;
    sc_signal<TensorData>* tensor_output_sig;
    
    // Signals for memory subsystem
    sc_signal<MemoryTransaction>* memory_transaction_sig;
    sc_signal<MemoryResponse>* memory_response_sig;
};

#endif // SHADER_CORE_H