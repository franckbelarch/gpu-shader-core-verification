#ifndef MEMORY_STATE_H
#define MEMORY_STATE_H

#include <cstdint>
#include <unordered_map>
#include <vector>

// Memory state class to track memory subsystem state
class MemoryState {
public:
    // Constructor
    MemoryState();
    
    // Memory access tracking
    void track_read(uint64_t address, uint32_t size);
    void track_write(uint64_t address, uint32_t size);
    void track_atomic(uint64_t address, uint32_t size);
    
    // Memory statistics
    uint64_t total_reads() const { return total_reads_; }
    uint64_t total_writes() const { return total_writes_; }
    uint64_t total_atomics() const { return total_atomics_; }
    
    // Memory hotspot analysis
    struct HotspotInfo {
        uint64_t address;
        uint64_t access_count;
        uint64_t last_access_time;
    };
    
    std::vector<HotspotInfo> get_hotspots(uint32_t top_n = 10) const;
    
    // Memory bandwidth tracking
    void update_bandwidth_usage(uint64_t bytes_transferred);
    double current_bandwidth_usage() const { return current_bandwidth_; }
    double peak_bandwidth_usage() const { return peak_bandwidth_; }
    
    // Memory latency tracking
    void track_latency(uint32_t cycles);
    double average_latency() const;
    uint32_t max_latency() const { return max_latency_; }
    
    // Reset statistics
    void reset_stats();
    
private:
    // Access counters
    uint64_t total_reads_;
    uint64_t total_writes_;
    uint64_t total_atomics_;
    
    // Access tracking
    std::unordered_map<uint64_t, uint64_t> address_access_count_;
    std::unordered_map<uint64_t, uint64_t> address_last_access_;
    
    // Bandwidth tracking
    double current_bandwidth_;
    double peak_bandwidth_;
    uint64_t current_cycle_;
    uint64_t bandwidth_window_size_;
    std::vector<uint64_t> bandwidth_history_;
    
    // Latency tracking
    uint64_t total_latency_cycles_;
    uint64_t total_latency_samples_;
    uint32_t max_latency_;
};

#endif // MEMORY_STATE_H