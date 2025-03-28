#ifndef MEMORY_RESPONSE_H
#define MEMORY_RESPONSE_H

#include <systemc.h>
#include <cstdint>
#include <iostream>

// Memory response status
enum class MemoryResponseStatus {
    OK,
    ERROR_ACCESS,
    ERROR_ALIGNMENT,
    ERROR_ADDRESS,
    ERROR_SIZE,
    ERROR_TIMEOUT,
    WAITING
};

// Memory response class
class MemoryResponse {
public:
    // Constructor
    MemoryResponse();
    MemoryResponse(uint32_t id, MemoryResponseStatus status);
    
    // Response properties
    uint32_t id() const { return id_; }
    MemoryResponseStatus status() const { return status_; }
    void set_status(MemoryResponseStatus status) { status_ = status; }
    
    // Data access
    void set_data(uint64_t data) { data_ = data; }
    uint64_t data() const { return data_; }
    
    // Latency information
    void set_latency(uint32_t cycles) { latency_cycles_ = cycles; }
    uint32_t latency() const { return latency_cycles_; }
    
    // Response is valid flag
    bool is_valid() const { return valid_; }
    void set_valid(bool valid) { valid_ = valid; }
    
    // SystemC conversion functions
    void operator = (const sc_in<MemoryResponse>& port);
    void operator = (const sc_out<MemoryResponse>& port);
    
    // Comparison operators for SystemC
    bool operator == (const MemoryResponse& other) const {
        return id_ == other.id_ && 
               status_ == other.status_ && 
               data_ == other.data_ && 
               latency_cycles_ == other.latency_cycles_ && 
               valid_ == other.valid_;
    }
    
    bool operator != (const MemoryResponse& other) const {
        return !(*this == other);
    }
    
private:
    uint32_t id_;
    MemoryResponseStatus status_;
    uint64_t data_;
    uint32_t latency_cycles_;
    bool valid_;
};

// SystemC type traits for MemoryResponse
namespace sc_core {
    // Forward declarations for external implementation
    void sc_trace(sc_trace_file* tf, const MemoryResponse& resp, const std::string& name);
    std::ostream& operator<<(std::ostream& os, const MemoryResponse& resp);
}

#endif // MEMORY_RESPONSE_H