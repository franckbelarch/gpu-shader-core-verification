#ifndef TENSOR_DATA_H
#define TENSOR_DATA_H

#include <systemc.h>
#include <vector>
#include <cstdint>

// Forward declarations
class TensorData;

// Different precision types for ML workloads
using fp4_t = uint8_t; // 4-bit floating point (packed)
using fp8_t = uint8_t; // 8-bit floating point
using fp16_t = uint16_t; // 16-bit floating point (half precision)

// Tensor data class that supports different precision formats
class TensorData {
public:
    enum class Precision {
        FP4,
        FP8,
        FP16,
        FP32  // Standard single precision float
    };

    // Constructors
    TensorData();
    TensorData(const std::vector<size_t>& dims, Precision precision = Precision::FP32);
    TensorData(const TensorData& other);
    
    // Get dimensions and other properties
    const std::vector<size_t>& dimensions() const { return dims_; }
    size_t size() const { return total_elements_; }
    Precision precision() const { return precision_; }
    
    // Data access methods
    void set_fp32(size_t index, float value);
    float get_fp32(size_t index) const;
    
    void set_fp16(size_t index, fp16_t value);
    fp16_t get_fp16(size_t index) const;
    
    void set_fp8(size_t index, fp8_t value);
    fp8_t get_fp8(size_t index) const;
    
    void set_fp4(size_t index, fp4_t value);  // Will pack two fp4 values per byte
    fp4_t get_fp4(size_t index) const;
    
    // Resize and change precision
    void resize(const std::vector<size_t>& dimensions);
    void change_precision(Precision new_precision);
    
    // SystemC conversion functions
    void operator = (const sc_in<TensorData>& port);
    void operator = (const sc_out<TensorData>& port);
    
    // Comparison operators for SystemC
    bool operator == (const TensorData& other) const {
        return dims_ == other.dims_ && 
               precision_ == other.precision_ && 
               total_elements_ == other.total_elements_;
        // Note: we compare structural properties only, not data
        // For full data comparison, we would need to check the appropriate
        // data array based on precision
    }
    
    bool operator != (const TensorData& other) const {
        return !(*this == other);
    }
    
private:
    std::vector<size_t> dims_;        // Dimensions of the tensor
    size_t total_elements_;           // Total number of elements
    Precision precision_;             // Precision of the data
    
    // Data storage for different precisions
    std::vector<float> data_fp32_;
    std::vector<fp16_t> data_fp16_;
    std::vector<fp8_t> data_fp8_;
    std::vector<fp4_t> data_fp4_;
    
    // Helper function to calculate total elements from dimensions
    size_t calculate_total_elements() const;
    
    // Helper functions for format conversion
    float fp16_to_fp32(fp16_t value) const;
    fp16_t fp32_to_fp16(float value) const;
    
    float fp8_to_fp32(fp8_t value) const;
    fp8_t fp32_to_fp8(float value) const;
    
    float fp4_to_fp32(fp4_t value, bool is_upper) const;
    fp4_t fp32_to_fp4(float value) const;
};

// Forward declarations for SystemC
namespace sc_core {
    // Define a proper function for tracing TensorData
    void sc_trace(sc_trace_file* tf, const TensorData& tensor, const std::string& name);
    
    // Stream output operator 
    std::ostream& operator<<(std::ostream& os, const TensorData& tensor);
}

#endif // TENSOR_DATA_H