#ifndef INSTRUCTION_BUFFER_H
#define INSTRUCTION_BUFFER_H

#include <systemc.h>
#include <queue>
#include <cstdint>

// Instruction buffer class
class InstructionBuffer : public sc_module {
public:
    // Ports
    sc_in<bool> clk;
    sc_in<bool> reset;
    
    // Input port (from instruction fetch)
    sc_in<bool> push_enable;
    sc_in<sc_uint<32>> push_instruction;
    sc_out<bool> push_ready;
    
    // Output port (to instruction decode)
    sc_in<bool> pop_enable;
    sc_out<sc_uint<32>> pop_instruction;
    sc_out<bool> pop_valid;
    
    // Constructor
    SC_HAS_PROCESS(InstructionBuffer);
    InstructionBuffer(sc_module_name name, uint32_t capacity = 16);
    
    // Processes
    void update_process();
    
    // Buffer state
    bool is_empty() const;
    bool is_full() const;
    uint32_t size() const;
    uint32_t capacity() const;
    
private:
    // Buffer storage
    std::queue<uint32_t> buffer_;
    uint32_t capacity_;
    
    // Internal state
    bool push_ready_;
    bool pop_valid_;
    uint32_t pop_instruction_;
};

#endif // INSTRUCTION_BUFFER_H