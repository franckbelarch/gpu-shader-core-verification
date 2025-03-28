# Examples CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# Add example executables
add_executable(basic_example basic_example.cpp)

# Link with verification environment and model
target_link_libraries(basic_example
  verification_env
  shader_core
  tensor_unit
  memory_subsystem
)