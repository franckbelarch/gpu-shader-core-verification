#ifndef EXECUTION_UNIT_H
#define EXECUTION_UNIT_H

#include <systemc.h>
#include "instruction_decoder.h"

// Execution unit class
class ExecutionUnit : public sc_module {
public:
    // Ports
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<DecodedInstruction> instruction;
    sc_in<sc_uint<32>> operand_a;
    sc_in<sc_uint<32>> operand_b;
    sc_out<sc_uint<32>> result;
    sc_out<bool> busy;
    
    // Constructor and destructor
    SC_HAS_PROCESS(ExecutionUnit);
    ExecutionUnit(sc_module_name name);
    ~ExecutionUnit();
    
    // Processes
    void execute_process();
    
    // ALU operations
    uint32_t execute_add(uint32_t a, uint32_t b);
    uint32_t execute_sub(uint32_t a, uint32_t b);
    uint32_t execute_mul(uint32_t a, uint32_t b);
    uint32_t execute_div(uint32_t a, uint32_t b);
    uint32_t execute_and(uint32_t a, uint32_t b);
    uint32_t execute_or(uint32_t a, uint32_t b);
    uint32_t execute_xor(uint32_t a, uint32_t b);
    uint32_t execute_not(uint32_t a);
    uint32_t execute_shl(uint32_t a, uint32_t b);
    uint32_t execute_shr(uint32_t a, uint32_t b);
    
private:
    // Execution state
    uint32_t current_result_;
    bool is_busy_;
    uint32_t execution_cycles_left_;
    
    // Internal signals for self-binding ports to avoid port binding errors
    sc_signal<sc_uint<32>>* internal_result_signal;
    sc_signal<bool>* internal_busy_signal;
    
    // Execution timing (in cycles)
    static const uint32_t ADD_CYCLES = 1;
    static const uint32_t SUB_CYCLES = 1;
    static const uint32_t MUL_CYCLES = 3;
    static const uint32_t DIV_CYCLES = 10;
    static const uint32_t LOGICAL_CYCLES = 1;
    static const uint32_t SHIFT_CYCLES = 1;
    
    // Helper methods
    uint32_t get_execution_cycles(const DecodedInstruction& instr);
};

#endif // EXECUTION_UNIT_H