#ifndef TRANSACTION_QUEUE_H
#define TRANSACTION_QUEUE_H

#include <queue>
#include <unordered_map>
#include <vector>
#include "memory_transaction.h"

// Transaction queue for memory subsystem
class TransactionQueue {
public:
    // Constructor
    TransactionQueue(uint32_t max_size = 32);
    
    // Queue operations
    bool push(const MemoryTransaction& transaction);
    bool pop(MemoryTransaction& transaction);
    bool peek(MemoryTransaction& transaction) const;
    
    // Queue state
    bool is_empty() const;
    bool is_full() const;
    uint32_t size() const;
    uint32_t capacity() const;
    
    // Transaction management
    bool has_transaction(uint32_t id) const;
    bool get_transaction(uint32_t id, MemoryTransaction& transaction) const;
    bool remove_transaction(uint32_t id);
    
    // Reordering capabilities
    void reorder_by_address();
    void reorder_by_id();
    
    // Conflict detection
    bool has_address_conflict(uint64_t address, uint32_t size) const;
    std::vector<uint32_t> get_conflicting_transactions(uint64_t address, uint32_t size) const;
    
    // Reset
    void clear();
    
private:
    uint32_t max_size_;
    std::queue<MemoryTransaction> queue_;
    std::unordered_map<uint32_t, MemoryTransaction> id_map_;
    std::unordered_map<uint64_t, std::vector<uint32_t>> address_map_;
    
    // Helper methods
    bool address_overlaps(uint64_t addr1, uint32_t size1, uint64_t addr2, uint32_t size2) const;
    void update_address_map(const MemoryTransaction& transaction, bool add);
};

#endif // TRANSACTION_QUEUE_H