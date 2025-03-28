#ifndef MEMORY_TRANSACTION_H
#define MEMORY_TRANSACTION_H

#include <systemc.h>
#include <cstdint>
#include <iostream>

// Memory transaction types
enum class MemoryTransactionType {
    READ,
    WRITE,
    ATOMIC_ADD,
    ATOMIC_AND,
    ATOMIC_OR,
    ATOMIC_XOR,
    ATOMIC_MIN,
    ATOMIC_MAX,
    ATOMIC_EXCH,
    ATOMIC_CAS
};

// Memory transaction class
class MemoryTransaction {
public:
    // Constructor
    MemoryTransaction();
    MemoryTransaction(MemoryTransactionType type, uint64_t address, uint32_t size = 4);
    
    // Transaction properties
    MemoryTransactionType type() const { return type_; }
    uint64_t address() const { return address_; }
    uint32_t size() const { return size_; }
    
    // Data access
    void set_data(uint64_t data) { data_ = data; }
    uint64_t data() const { return data_; }
    
    // Transaction ID
    void set_id(uint32_t id) { id_ = id; }
    uint32_t id() const { return id_; }
    
    // Transaction status
    bool is_pending() const { return pending_; }
    void set_pending(bool pending) { pending_ = pending; }
    
    // SystemC conversion functions
    void operator = (const sc_in<MemoryTransaction>& port);
    void operator = (const sc_out<MemoryTransaction>& port);
    
    // Comparison operators for SystemC
    bool operator == (const MemoryTransaction& other) const {
        return type_ == other.type_ && 
               address_ == other.address_ && 
               size_ == other.size_ && 
               data_ == other.data_ && 
               id_ == other.id_ && 
               pending_ == other.pending_;
    }
    
    bool operator != (const MemoryTransaction& other) const {
        return !(*this == other);
    }
    
private:
    MemoryTransactionType type_;
    uint64_t address_;
    uint32_t size_;
    uint64_t data_;
    uint32_t id_;
    bool pending_;
};

// SystemC type traits for MemoryTransaction
namespace sc_core {
    // Forward declarations for external implementation
    void sc_trace(sc_trace_file* tf, const MemoryTransaction& txn, const std::string& name);
    std::ostream& operator<<(std::ostream& os, const MemoryTransaction& txn);
}

#endif // MEMORY_TRANSACTION_H