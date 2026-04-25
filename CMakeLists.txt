cmake_minimum_required(VERSION 3.15)

project(video_labs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SANITIZE_THREAD "Build with ThreadSanitizer" OFF)
option(SANITIZE_ADDRESS "Build with AddressSanitizer" OFF)

if(SANITIZE_THREAD AND SANITIZE_ADDRESS)
    message(FATAL_ERROR "SANITIZE_THREAD and SANITIZE_ADDRESS cannot be enabled together")
endif()

add_library(video_labs_core INTERFACE)
target_include_directories(video_labs_core INTERFACE include)
target_compile_options(video_labs_core INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -fno-omit-frame-pointer
)

if(SANITIZE_THREAD)
    target_compile_options(video_labs_core INTERFACE -fsanitize=thread)
    target_link_options(video_labs_core INTERFACE -fsanitize=thread)
endif()

if(SANITIZE_ADDRESS)
    target_compile_options(video_labs_core INTERFACE -fsanitize=address)
    target_link_options(video_labs_core INTERFACE -fsanitize=address)
endif()

add_executable(video_labs_phase1 src/main.cpp)
target_link_libraries(video_labs_phase1 PRIVATE video_labs_core)

enable_testing()

add_executable(video_labs_tests test/thread_safe_queue_test.cpp)
target_link_libraries(video_labs_tests PRIVATE video_labs_core)

add_test(NAME video_labs_tests COMMAND video_labs_tests)
