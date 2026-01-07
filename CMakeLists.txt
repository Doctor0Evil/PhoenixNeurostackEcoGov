# File: CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(PhoenixNeurostackEcoGov VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Executable
add_executable(PhoenixNeurostackEcoGov
    src/main.cpp
    src/loader.cpp
    src/eco_metrics.cpp
    src/cyberswarm_kernel.cpp
    src/dreamnet_index.cpp
    src/cybercore_policy.cpp
)

target_include_directories(PhoenixNeurostackEcoGov
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install target
install(TARGETS PhoenixNeurostackEcoGov
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Data directory
install(DIRECTORY data/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/data
    FILES_MATCHING PATTERN "*.csv"
)
