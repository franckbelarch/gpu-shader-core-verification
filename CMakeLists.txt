cmake_minimum_required(VERSION 3.10)
project(GPU_Shader_Core_Verification)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SystemC package
find_package(SystemCLanguage QUIET)
if(NOT SystemCLanguage_FOUND)
    # If SystemC is not found via CMake, try to find it manually
    if(DEFINED ENV{SYSTEMC_HOME})
        set(SystemC_HOME $ENV{SYSTEMC_HOME})
        include_directories(${SystemC_HOME}/include)
        link_directories(${SystemC_HOME}/lib-linux64)
    else()
        # Use system-installed SystemC
        include_directories(/usr/include)
        link_directories(/usr/lib/x86_64-linux-gnu)
        set(SystemC_HOME "/usr")
    endif()
endif()

# Check if coverage is enabled
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
if(ENABLE_COVERAGE)
    message(STATUS "Enabling coverage reporting")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Define include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/model
    ${CMAKE_SOURCE_DIR}/verification
)

# Add subdirectories
add_subdirectory(model)
add_subdirectory(verification)

# Create data directory for coverage and test history
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/data)

# Install scripts
install(DIRECTORY ${CMAKE_SOURCE_DIR}/infrastructure/scripts/
        DESTINATION bin
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING PATTERN "*.sh" PATTERN "*.py")

# Install Python tools
install(DIRECTORY ${CMAKE_SOURCE_DIR}/ai_tools/
        DESTINATION lib/ai_tools
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING PATTERN "*.py")

# Create empty coverage data file if it doesn't exist
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/data/coverage_data.json)
    file(WRITE ${CMAKE_SOURCE_DIR}/data/coverage_data.json "{}")
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "GPU Shader Core Verification Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  SystemC location: ${SystemC_HOME}")
message(STATUS "  Coverage: ${ENABLE_COVERAGE}")
message(STATUS "")