cmake_minimum_required(VERSION 3.16)
project(h1_2_arm_control_examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(UNITREE_SDK2_ROOT "" CACHE PATH "Path to the unitree_sdk2 source tree")
if(NOT UNITREE_SDK2_ROOT)
  message(FATAL_ERROR
    "UNITREE_SDK2_ROOT is required. Example: "
    "cmake -S . -B build -DUNITREE_SDK2_ROOT=/home/user/unitree_sdk2")
endif()

if(NOT EXISTS "${UNITREE_SDK2_ROOT}/CMakeLists.txt")
  message(FATAL_ERROR "No unitree_sdk2 CMakeLists.txt found under: ${UNITREE_SDK2_ROOT}")
endif()

set(BUILD_EXAMPLES OFF CACHE BOOL "Build unitree_sdk2 examples" FORCE)
add_subdirectory(
  "${UNITREE_SDK2_ROOT}"
  "${CMAKE_BINARY_DIR}/unitree_sdk2"
  EXCLUDE_FROM_ALL
)

add_executable(arm_state_monitor arm_state_monitor.cpp)
target_link_libraries(arm_state_monitor PRIVATE unitree_sdk2)

