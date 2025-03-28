#ifndef TEST_CASE_H
#define TEST_CASE_H

#include <string>
#include <vector>
#include <map>
#include <systemc.h>
#include "../../model/shader_core/shader_core.h"
#include "../checkers/scoreboard.h"
#include "../coverage/coverage_collector.h"
#include "../transaction.h"

// Forward declarations
class VerificationConfig;

// Test signal manager to create and manage signals for testing
class TestSignalManager {
public:
    TestSignalManager();
    ~TestSignalManager();
    
    // Get standard signals
    sc_signal<bool>& get_clk_signal();
    sc_signal<bool>& get_reset_signal();
    sc_signal<sc_uint<32>>& get_instruction_signal();
    sc_signal<sc_uint<32>>& get_operand_a_signal();
    sc_signal<sc_uint<32>>& get_operand_b_signal();
    sc_signal<sc_uint<32>>& get_result_signal();
    sc_signal<bool>& get_busy_signal();
    
    // Get tensor signals
    sc_signal<TensorOpcode>& get_tensor_opcode_signal();
    sc_signal<TensorData>& get_tensor_input_a_signal();
    sc_signal<TensorData>& get_tensor_input_b_signal();
    sc_signal<TensorData>& get_tensor_output_signal();
    
    // Get memory signals
    sc_signal<MemoryTransaction>& get_memory_transaction_signal();
    sc_signal<MemoryResponse>& get_memory_response_signal();
    
    // Connect signals to DUT
    void connect_signals_to_shader_core(ShaderCore* core);
    void connect_signals_to_tensor_unit(TensorUnit* unit);
    void connect_tensor_interface(ShaderCore* core);
    
    // Simulation helpers
    void reset_dut(int cycles = 5, sc_time cycle_time = sc_time(10, SC_NS));
    void toggle_clock(int cycles = 1, sc_time cycle_time = sc_time(10, SC_NS));
    void advance_simulation(sc_time time = sc_time(10, SC_NS));
    
private:
    // Basic signals
    sc_signal<bool>* clk_signal_;
    sc_signal<bool>* reset_signal_;
    sc_signal<sc_uint<32>>* instruction_signal_;
    sc_signal<sc_uint<32>>* operand_a_signal_;
    sc_signal<sc_uint<32>>* operand_b_signal_;
    sc_signal<sc_uint<32>>* result_signal_;
    sc_signal<bool>* busy_signal_;
    
    // Tensor signals
    sc_signal<TensorOpcode>* tensor_opcode_signal_;
    sc_signal<TensorData>* tensor_input_a_signal_;
    sc_signal<TensorData>* tensor_input_b_signal_;
    sc_signal<TensorData>* tensor_output_signal_;
    
    // Memory signals
    sc_signal<MemoryTransaction>* memory_transaction_signal_;
    sc_signal<MemoryResponse>* memory_response_signal_;
};

// Base test case class
class TestCase {
public:
    // Test configuration
    std::string name;
    VerificationConfig* config;
    
    // Constructor
    TestCase(const std::string& test_name);
    virtual ~TestCase();
    
    // Required methods
    virtual void setup() = 0;
    virtual void execute() = 0;
    virtual void cleanup() = 0;
    
    // Analysis methods
    virtual bool verify_results();
    virtual void report_statistics();
    
    // Resource setters
    void set_dut(ShaderCore* dut) { dut_ = dut; }
    void set_reference(ShaderCore* ref) { reference_ = ref; }
    void set_scoreboard(Scoreboard* scoreboard) { scoreboard_ = scoreboard; }
    void set_coverage(CoverageCollector* coverage) { coverage_ = coverage; }
    
    // Status and reporting
    bool passed() const { return passed_; }
    void set_passed(bool passed) { passed_ = passed; }
    void log_message(const std::string& message);
    const std::vector<std::string>& get_log() const { return log_messages_; }
    
    // Test results
    struct TestResult {
        bool passed;
        std::map<std::string, double> metrics;
        std::vector<std::string> messages;
    };
    
    const TestResult& get_results() const { return results_; }
    void add_result_metric(const std::string& name, double value);
    
protected:
    // Test resources
    ShaderCore* dut_;
    ShaderCore* reference_;
    Scoreboard* scoreboard_;
    CoverageCollector* coverage_;
    TestSignalManager* signal_manager_;
    
    // Test status
    bool passed_;
    TestResult results_;
    std::vector<std::string> log_messages_;
    
    // Friend declarations
    friend class BasicTensorTestCase;
};

// Specialized test case for ALU operations
class ShaderALUTest : public TestCase {
public:
    ShaderALUTest(const std::string& test_name);
    virtual ~ShaderALUTest();
    
    virtual void setup() override;
    virtual void execute() override;
    virtual void cleanup() override;
    
private:
    // Test-specific data
    std::vector<sc_uint<32>> test_instructions_;
    std::vector<sc_uint<32>> test_operands_a_;
    std::vector<sc_uint<32>> test_operands_b_;
    std::vector<sc_uint<32>> expected_results_;
};

// Specialized test case for tensor operations
class TensorCoreTest : public TestCase {
public:
    TensorCoreTest(const std::string& test_name);
    virtual ~TensorCoreTest();
    
    virtual void setup() override;
    virtual void execute() override;
    virtual void cleanup() override;
    
    // Allow for direct tensor unit testing
    TensorUnit* tensor_unit_override;
    
protected:
    // Test-specific data
    std::vector<TensorData> test_tensors_a_;
    std::vector<TensorData> test_tensors_b_;
    std::vector<TensorOpcode> test_opcodes_;
    std::vector<TensorData> expected_results_;
};

// Specialized test case for memory subsystem
class MemorySubsystemTest : public TestCase {
public:
    MemorySubsystemTest(const std::string& test_name);
    virtual ~MemorySubsystemTest();
    
    virtual void setup() override;
    virtual void execute() override;
    virtual void cleanup() override;
    
private:
    // Test-specific data
    std::vector<MemoryTransaction> test_transactions_;
    std::vector<MemoryResponse> expected_responses_;
};

// Specialized test case for edge AI optimization
class EdgeAIInferenceTest : public TestCase {
public:
    EdgeAIInferenceTest(const std::string& test_name);
    virtual ~EdgeAIInferenceTest();
    
    virtual void setup() override;
    virtual void execute() override;
    virtual void cleanup() override;
    
private:
    // Test-specific data
    TensorData input_tensor_;
    TensorData expected_output_;
    std::vector<uint32_t> instructions_;
};

#endif // TEST_CASE_H