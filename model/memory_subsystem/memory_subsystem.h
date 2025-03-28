#ifndef MEMORY_SUBSYSTEM_H
#define MEMORY_SUBSYSTEM_H

#include <systemc.h>
#include "memory_transaction.h"
#include "memory_response.h"
#include "register_file.h"
#include "shared_memory.h"
#include "cache_l1.h"
#include "cache_l2.h"
#include "global_memory.h"
#include "memory_state.h"
#include "transaction_queue.h"

class MemorySubsystem : public sc_module {
public:
    // Ports
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<MemoryTransaction> transaction;
    sc_out<MemoryResponse> response;
    
    // Internal signals for component connections
    sc_signal<MemoryTransaction> reg_txn_signal;
    sc_signal<MemoryResponse> reg_resp_signal;
    sc_signal<MemoryTransaction> shared_txn_signal;
    sc_signal<MemoryResponse> shared_resp_signal;
    sc_signal<MemoryTransaction> l1_txn_signal;
    sc_signal<MemoryResponse> l1_resp_signal;
    sc_signal<MemoryTransaction> l1_to_l2_txn_signal;
    sc_signal<MemoryResponse> l2_to_l1_resp_signal;
    sc_signal<MemoryTransaction> l2_to_global_txn_signal;
    sc_signal<MemoryResponse> global_to_l2_resp_signal;
    
    // Constructor
    SC_HAS_PROCESS(MemorySubsystem);
    MemorySubsystem(sc_module_name name);
    
    // Memory hierarchy components
    RegisterFile* registers;
    SharedMemory* shared_memory;
    CacheL1* l1_cache;
    CacheL2* l2_cache;
    GlobalMemory* global_memory;
    
    // Memory processes
    void handle_transaction();
    void update_caches();
    
    // HBM3e-specific methods
    void simulate_hbm3e_bandwidth();
    void simulate_hbm3e_latency();
    
private:
    // Memory state
    MemoryState state;
    TransactionQueue pending_transactions;
};

#endif // MEMORY_SUBSYSTEM_H