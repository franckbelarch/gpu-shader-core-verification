#ifndef INSTRUCTION_DECODER_H
#define INSTRUCTION_DECODER_H

#include <systemc.h>
#include <cstdint>

// Instruction opcodes
enum class InstructionOpcode {
    // ALU operations
    ALU_ADD,
    ALU_SUB,
    ALU_MUL,
    ALU_DIV,
    ALU_AND,
    ALU_OR,
    ALU_XOR,
    ALU_NOT,
    ALU_SHL,
    ALU_SHR,
    
    // Tensor operations
    TENSOR_MATMUL_FP16,
    TENSOR_MATMUL_FP8,
    TENSOR_MATMUL_FP4,
    TENSOR_CONV2D,
    TENSOR_ATTENTION,
    
    // Memory operations
    MEM_LOAD,
    MEM_STORE,
    MEM_ATOMIC,
    
    // Control flow
    BRANCH,
    BRANCH_COND,
    JUMP,
    CALL,
    RETURN,
    
    // Special operations
    BARRIER,
    SYNC,
    NOP
};

// Comparison operators for enums are already provided by the compiler

// Decoded instruction structure
struct DecodedInstruction {
    InstructionOpcode opcode;
    uint32_t src_reg1;
    uint32_t src_reg2;
    uint32_t dst_reg;
    uint32_t immediate;
    bool uses_immediate;
    bool is_predicated;
    uint32_t predicate_reg;
    
    // Comparison operators for SystemC
    bool operator==(const DecodedInstruction& other) const {
        return opcode == other.opcode &&
               src_reg1 == other.src_reg1 &&
               src_reg2 == other.src_reg2 &&
               dst_reg == other.dst_reg &&
               immediate == other.immediate &&
               uses_immediate == other.uses_immediate &&
               is_predicated == other.is_predicated &&
               predicate_reg == other.predicate_reg;
    }
    
    bool operator!=(const DecodedInstruction& other) const {
        return !(*this == other);
    }
};

// Instruction decoder class
class InstructionDecoder : public sc_module {
public:
    // Ports
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<sc_uint<32>> instruction;
    sc_out<DecodedInstruction> decoded_instr;
    
    // Constructor
    SC_HAS_PROCESS(InstructionDecoder);
    InstructionDecoder(sc_module_name name);
    
    // Process
    void decode_process();
    
private:
    // Instruction format constants
    static const uint32_t OPCODE_MASK = 0xFC000000;
    static const uint32_t OPCODE_SHIFT = 26;
    static const uint32_t REG_DST_MASK = 0x03E00000;
    static const uint32_t REG_DST_SHIFT = 21;
    static const uint32_t REG_SRC1_MASK = 0x001F0000;
    static const uint32_t REG_SRC1_SHIFT = 16;
    static const uint32_t REG_SRC2_MASK = 0x0000F800;
    static const uint32_t REG_SRC2_SHIFT = 11;
    static const uint32_t IMM_MASK = 0x0000FFFF;
    static const uint32_t PRED_MASK = 0x03800000;
    static const uint32_t PRED_SHIFT = 23;
    
    // Helper methods
    InstructionOpcode decode_opcode(uint32_t raw_instr);
    DecodedInstruction decode_instruction(uint32_t raw_instr);
};

// SystemC tracing for DecodedInstruction
namespace sc_core {
    // Forward declarations for external implementation
    void sc_trace(sc_trace_file* tf, const DecodedInstruction& instr, const std::string& name);
    std::ostream& operator<<(std::ostream& os, const DecodedInstruction& instr);
}

#endif // INSTRUCTION_DECODER_H