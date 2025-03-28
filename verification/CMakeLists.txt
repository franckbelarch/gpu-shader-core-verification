# Verification components build configuration

# Find Google Test
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    # If GTest is not found, download and build it
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
endif(NOT GTest_FOUND)

# Add verification libraries
add_library(verification_env
    verification_environment.cpp
    transaction.cpp
    monitors/monitor.cpp
    checkers/scoreboard.cpp
    coverage/coverage_collector.cpp
    test_cases/test_case.cpp
)

# Set library properties and link with model and SystemC
target_link_libraries(verification_env PUBLIC
    gpu_shader_model
    gtest
    pthread
)

# Create test executables

# Basic tests
add_executable(basic_alu_tests test_cases/basic_alu_test.cpp)
target_link_libraries(basic_alu_tests PRIVATE verification_env)

add_executable(basic_tensor_tests test_cases/basic_tensor_test.cpp)
target_link_libraries(basic_tensor_tests PRIVATE verification_env)

add_executable(basic_memory_tests test_cases/basic_memory_test.cpp)
target_link_libraries(basic_memory_tests PRIVATE verification_env)

add_executable(edge_ai_tests test_cases/edge_ai_test.cpp)
target_link_libraries(edge_ai_tests PRIVATE verification_env)

# Create test runner script
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/run_all_tests.sh.in
    ${CMAKE_BINARY_DIR}/run_all_tests.sh
    @ONLY
)

# Make test runner executable
file(CHMOD ${CMAKE_BINARY_DIR}/run_all_tests.sh
     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)