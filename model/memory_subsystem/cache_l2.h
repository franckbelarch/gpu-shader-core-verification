#ifndef CACHE_L2_H
#define CACHE_L2_H

#include <systemc.h>
#include <vector>
#include <unordered_map>
#include <list>
#include <cstdint>
#include "memory_transaction.h"
#include "memory_response.h"

// L2 Cache implementation
class CacheL2 : public sc_module {
public:
    // Ports for upper level (L1 cache)
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<MemoryTransaction> transaction_in;
    sc_out<MemoryResponse> response_out;
    
    // Ports for lower level (global memory)
    sc_out<MemoryTransaction> transaction_out;
    sc_in<MemoryResponse> response_in;
    
    // Constructor
    SC_HAS_PROCESS(CacheL2);
    CacheL2(sc_module_name name, 
           uint32_t size_bytes = 2 * 1024 * 1024,  // Default 2MB
           uint32_t line_size = 128,               // Default 128B cache line
           uint32_t associativity = 8);            // Default 8-way associative
    
    // Destructor
    virtual ~CacheL2();
    
    // Process methods
    void handle_requests();
    void handle_responses();
    
    // Cache properties
    uint32_t size() const { return size_bytes_; }
    uint32_t line_size() const { return line_size_; }
    uint32_t associativity() const { return associativity_; }
    
    // Cache statistics
    uint64_t hits() const { return hits_; }
    uint64_t misses() const { return misses_; }
    double hit_rate() const;
    
    // Reset statistics
    void reset_stats();
    
private:
    // Cache configuration
    uint32_t size_bytes_;
    uint32_t line_size_;
    uint32_t associativity_;
    uint32_t num_sets_;
    
    // Cache statistics
    uint64_t hits_;
    uint64_t misses_;
    
    // Cache line definition
    struct CacheLine {
        bool valid;
        bool dirty;
        uint64_t tag;
        std::vector<uint8_t> data;
        uint64_t last_access_time;
        
        // Coherence state (for future extensions)
        enum class State { INVALID, SHARED, EXCLUSIVE, MODIFIED } state;
    };
    
    // Cache storage - sets of cache lines
    std::vector<std::vector<CacheLine>> cache_sets_;
    
    // Pending transactions waiting for responses from global memory
    std::vector<MemoryTransaction> pending_transactions_;
    
    // MSHR (Miss Status Handling Register) for tracking in-flight misses
    struct MSHR {
        uint32_t transaction_id;
        MemoryTransaction original_transaction;
        uint64_t addr;
        uint32_t set_index;
        uint64_t tag;
        bool pending;
        std::vector<uint32_t> dependent_transactions;  // IDs of transactions depending on this miss
    };
    
    std::unordered_map<uint32_t, MSHR> mshr_entries_;
    std::unordered_map<uint64_t, uint32_t> addr_to_mshr_;  // Maps block addresses to MSHR entries
    uint32_t max_mshr_entries_;
    
    // Transaction counter
    uint32_t transaction_id_;
    uint64_t current_cycle_;
    
    // Helper methods
    bool lookup(uint64_t addr, uint32_t& set_index, uint32_t& line_index);
    void handle_hit(const MemoryTransaction& transaction, uint32_t set_index, uint32_t line_index);
    void handle_miss(const MemoryTransaction& transaction);
    void allocate_line(uint32_t set_index, uint64_t tag, const std::vector<uint8_t>& data);
    uint32_t select_victim(uint32_t set_index);
    void evict_line(uint32_t set_index, uint32_t line_index);
    
    // Address translation helpers
    uint32_t get_set_index(uint64_t addr) const;
    uint64_t get_tag(uint64_t addr) const;
    uint32_t get_offset(uint64_t addr) const;
    uint64_t get_block_addr(uint64_t addr) const;
    
    // MSHR handling
    bool check_mshr_hit(uint64_t block_addr, uint32_t& mshr_id);
    bool allocate_mshr(const MemoryTransaction& transaction, uint32_t& mshr_id);
    void handle_mshr_response(uint32_t mshr_id, const std::vector<uint8_t>& data);
};

#endif // CACHE_L2_H