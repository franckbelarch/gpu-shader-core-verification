#ifndef CACHE_L1_H
#define CACHE_L1_H

#include <systemc.h>
#include <vector>
#include <unordered_map>
#include <list>
#include <cstdint>
#include "memory_transaction.h"
#include "memory_response.h"

// L1 Cache implementation
class CacheL1 : public sc_module {
public:
    // Ports for upper level (shader core)
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<MemoryTransaction> transaction_in;
    sc_out<MemoryResponse> response_out;
    
    // Ports for lower level (L2 cache)
    sc_out<MemoryTransaction> transaction_out;
    sc_in<MemoryResponse> response_in;
    
    // Constructor
    SC_HAS_PROCESS(CacheL1);
    CacheL1(sc_module_name name, 
           uint32_t size_bytes = 64 * 1024,  // Default 64KB
           uint32_t line_size = 64,          // Default 64B cache line
           uint32_t associativity = 4);      // Default 4-way associative
           
    // Destructor
    virtual ~CacheL1();
    
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
    };
    
    // Cache storage - sets of cache lines
    std::vector<std::vector<CacheLine>> cache_sets_;
    
    // Pending transactions waiting for responses from L2
    std::vector<MemoryTransaction> pending_transactions_;
    
    // MSHR (Miss Status Handling Register) for tracking in-flight misses
    struct MSHR {
        uint32_t transaction_id;
        MemoryTransaction original_transaction;
        uint64_t addr;
        uint32_t set_index;
        uint64_t tag;
        bool pending;
    };
    
    std::unordered_map<uint32_t, MSHR> mshr_entries_;
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
};

#endif // CACHE_L1_H