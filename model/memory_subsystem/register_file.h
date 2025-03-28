#ifndef MEMORY_REGISTER_FILE_H
#define MEMORY_REGISTER_FILE_H

#include <systemc.h>
#include <vector>
#include <cstdint>
#include "memory_transaction.h"
#include "memory_response.h"

// Memory subsystem register file
class RegisterFile : public sc_module {
public:
    // Ports
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<MemoryTransaction> transaction;
    sc_out<MemoryResponse> response;
    
    // Constructor
    SC_HAS_PROCESS(RegisterFile);
    RegisterFile(sc_module_name name, uint32_t size = 256);
    
    // Process methods
    void handle_transaction();
    
    // Direct access for testing and initialization
    uint32_t read(uint32_t addr) const;
    void write(uint32_t addr, uint32_t data);
    
private:
    // Register storage
    std::vector<uint32_t> registers_;
    
    // Transaction counter
    uint32_t transaction_id_;
    
    // Performance parameters
    static const uint32_t READ_LATENCY = 1;
    static const uint32_t WRITE_LATENCY = 1;
};

#endif // MEMORY_REGISTER_FILE_H