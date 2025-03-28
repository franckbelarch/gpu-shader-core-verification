# Model components build configuration

# Add shader core library
add_library(shader_core
    shader_core/shader_core.cpp
    shader_core/instruction_decoder.cpp
    shader_core/register_file.cpp
    shader_core/execution_unit.cpp
    shader_core/instruction_buffer.cpp
)

# Add tensor unit library
add_library(tensor_unit
    tensor_unit/tensor_unit.cpp
    tensor_unit/tensor_data.cpp
    tensor_unit/tensor_buffer.cpp
    tensor_unit/tensor_opcode.cpp
)

# Add memory subsystem library
add_library(memory_subsystem
    memory_subsystem/memory_subsystem.cpp
    memory_subsystem/memory_transaction.cpp
    memory_subsystem/memory_response.cpp
    memory_subsystem/memory_state.cpp
    memory_subsystem/transaction_queue.cpp
    memory_subsystem/register_file.cpp
    memory_subsystem/shared_memory.cpp
    memory_subsystem/cache_l1.cpp
    memory_subsystem/cache_l2.cpp
    memory_subsystem/global_memory.cpp
)

# Set library properties and link with SystemC
target_link_libraries(shader_core PUBLIC tensor_unit memory_subsystem systemc-2.3.3)
target_link_libraries(tensor_unit PUBLIC systemc-2.3.3)
target_link_libraries(memory_subsystem PUBLIC systemc-2.3.3)

# Create a combined library for the entire model
add_library(gpu_shader_model INTERFACE)
target_link_libraries(gpu_shader_model INTERFACE
    shader_core
    tensor_unit
    memory_subsystem
)