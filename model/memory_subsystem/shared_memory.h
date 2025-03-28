#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <systemc.h>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "memory_transaction.h"
#include "memory_response.h"

// Shared memory class
class SharedMemory : public sc_module {
public:
    // Ports
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<MemoryTransaction> transaction;
    sc_out<MemoryResponse> response;
    
    // Constructor
    SC_HAS_PROCESS(SharedMemory);
    SharedMemory(sc_module_name name, uint32_t size_bytes = 64 * 1024);  // Default 64KB
    
    // Process methods
    void handle_transaction();
    
    // Direct memory access for testing
    uint32_t read(uint32_t addr) const;
    void write(uint32_t addr, uint32_t data);
    
    // Memory properties
    uint32_t size() const { return memory_.size(); }
    
private:
    // Memory storage (byte addressable)
    std::vector<uint8_t> memory_;
    
    // Transaction counter
    uint32_t transaction_id_;
    
    // Bank conflict tracking for performance simulation
    struct BankState {
        bool busy;
        uint32_t busy_until_cycle;
    };
    
    std::vector<BankState> bank_states_;
    uint32_t current_cycle_;
    
    // Shared memory configuration
    static const uint32_t NUM_BANKS = 32;
    static const uint32_t BANK_SIZE_BYTES = 4;
    static const uint32_t BASE_READ_LATENCY = 1;
    static const uint32_t BASE_WRITE_LATENCY = 1;
    static const uint32_t BANK_CONFLICT_PENALTY = 1;
    
    // Helper methods
    uint32_t get_bank_index(uint32_t addr) const;
    uint32_t calculate_latency(const MemoryTransaction& transaction);
    bool check_bank_conflicts(const MemoryTransaction& transaction, std::vector<uint32_t>& conflict_banks);
    void update_bank_states(const std::vector<uint32_t>& banks, uint32_t cycles);
};

#endif // SHARED_MEMORY_H