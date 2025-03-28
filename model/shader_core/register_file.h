#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <systemc.h>
#include <vector>
#include <cstdint>

// Shader Register file class - renamed to avoid conflict with memory subsystem RegisterFile
class ShaderRegisterFile : public sc_module {
public:
    // Ports
    sc_in<bool> clk;
    sc_in<bool> reset;
    
    // Read ports
    sc_in<sc_uint<8>> read_addr1;
    sc_in<sc_uint<8>> read_addr2;
    sc_out<sc_uint<32>> read_data1;
    sc_out<sc_uint<32>> read_data2;
    
    // Write port
    sc_in<bool> write_enable;
    sc_in<sc_uint<8>> write_addr;
    sc_in<sc_uint<32>> write_data;
    
    // Constructor
    SC_HAS_PROCESS(ShaderRegisterFile);
    ShaderRegisterFile(sc_module_name name, uint32_t num_registers = 256);
    
    // Processes
    void read_process();
    void write_process();
    
    // Direct register access for testing and initialization
    uint32_t read_register(uint32_t addr) const;
    void write_register(uint32_t addr, uint32_t data);
    
    // Register file info
    uint32_t size() const { return registers_.size(); }
    
private:
    // Register storage
    std::vector<uint32_t> registers_;
};

#endif // REGISTER_FILE_H