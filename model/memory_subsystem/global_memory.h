#ifndef GLOBAL_MEMORY_H
#define GLOBAL_MEMORY_H

#include <systemc.h>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include "memory_transaction.h"
#include "memory_response.h"

// Global memory with HBM3e simulation
class GlobalMemory : public sc_module {
public:
    // Ports
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<MemoryTransaction> transaction;
    sc_out<MemoryResponse> response;
    
    // Constructor
    SC_HAS_PROCESS(GlobalMemory);
    GlobalMemory(sc_module_name name, 
                uint64_t size_bytes = 16ULL * 1024 * 1024 * 1024,  // Default 16GB
                uint32_t num_channels = 8,                         // Default 8 channels
                uint32_t channel_width_bits = 256);                // Default 256-bit wide channels
    
    // Destructor
    virtual ~GlobalMemory();
    
    // Process methods
    void handle_transaction();
    void update_channels();
    
    // Direct memory access for testing
    uint8_t read_byte(uint64_t addr) const;
    void write_byte(uint64_t addr, uint8_t data);
    
    // HBM3e specific methods
    double current_bandwidth_usage() const;
    double peak_bandwidth() const;
    double average_latency() const;
    
    // Memory properties
    uint64_t size() const { return memory_.size(); }
    
    // Reset statistics
    void reset_stats();
    
private:
    // Memory storage (byte addressable)
    std::vector<uint8_t> memory_;
    uint64_t size_bytes_;
    uint32_t latency_ns_;
    
    // Pending transaction tracking
    struct PendingTransaction {
        MemoryTransaction txn;
        uint32_t cycles_left;
    };
    
    std::vector<PendingTransaction> pending_transactions_;
    
    // Memory channel simulation
    struct MemoryChannel {
        bool busy;
        uint64_t busy_until_cycle;
        std::queue<MemoryTransaction> pending_transactions;
        uint64_t bytes_transferred;
    };
    
    std::vector<MemoryChannel> channels_;
    std::unordered_map<uint32_t, uint32_t> transaction_to_channel_;
    
    // Transaction counter and timing
    uint32_t transaction_id_;
    uint64_t current_cycle_;
    
    // HBM3e configuration
    uint32_t num_channels_;
    uint32_t channel_width_bits_;
    uint64_t max_bandwidth_gbps_;
    uint32_t base_latency_cycles_;
    
    // Performance tracking
    uint64_t total_bytes_transferred_;
    uint64_t total_latency_cycles_;
    uint64_t total_transactions_;
    uint64_t current_bandwidth_gbps_;
    
    // Helper methods
    uint32_t get_channel_index(uint64_t addr) const;
    uint32_t calculate_transaction_latency(const MemoryTransaction& transaction) const;
    void schedule_transaction(const MemoryTransaction& transaction);
    void complete_transaction(uint32_t channel_idx);
    
    // HBM3e simulation methods
    uint32_t simulate_hbm3e_latency(const MemoryTransaction& txn);
    uint32_t simulate_hbm3e_bandwidth(const MemoryTransaction& txn);
    
    // HBM3e specific parameters
    static const uint32_t HBM3E_BASE_LATENCY = 100;  // Base latency in cycles
    static const uint32_t HBM3E_BANDWIDTH_PER_CHANNEL_GBPS = 24;  // 24 Gbps per channel for HBM3e
    static const uint32_t HBM3E_BURST_LENGTH = 16;  // Burst length in cycles
};

#endif // GLOBAL_MEMORY_H