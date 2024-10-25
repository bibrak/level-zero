# Process Abseil's CMake build system
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/abseil-cpp)

target_sources(${TARGET_NAME} 
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/zel_events_deadlock_checker.h
        ${CMAKE_CURRENT_LIST_DIR}/zel_events_deadlock_checker.cpp

        ${CMAKE_SOURCE_DIR}/third_party/xla/graphcycles.cc
)

target_include_directories(${TARGET_NAME} 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/third_party
)

cmake_policy(SET CMP0079 NEW)

# Declare dependency on the absl:: libraries
target_link_libraries(${TARGET_NAME} 
        absl::strings
        absl::container_memory
        absl::hash_container_defaults
        absl::raw_hash_set
        absl::algorithm_container
        absl::core_headers
        absl::memory
        absl::type_traits
)