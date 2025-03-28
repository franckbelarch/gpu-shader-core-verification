#ifndef TENSOR_BUFFER_H
#define TENSOR_BUFFER_H

#include <systemc.h>
#include "tensor_data.h"
#include <queue>

// A buffer for storing and managing tensor data
class TensorBuffer {
public:
    // Constructor
    TensorBuffer(size_t capacity = 8);
    
    // Buffer operations
    bool push(const TensorData& data);
    bool pop(TensorData& data);
    const TensorData& peek() const;
    bool is_empty() const;
    bool is_full() const;
    size_t size() const;
    size_t capacity() const;
    
    // SystemC integration
    void update();
    void reset();
    
private:
    size_t capacity_;
    std::queue<TensorData> buffer_;
};

#endif // TENSOR_BUFFER_H