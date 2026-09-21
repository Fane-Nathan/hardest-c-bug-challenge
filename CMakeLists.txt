cmake_minimum_required(VERSION 3.16)
project(hardest_c_bug_challenge C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(hardest_c_bug
    src/main.c
    src/store.c
    src/plan.c
    src/loader.c
    src/noise.c
)

target_include_directories(hardest_c_bug PRIVATE include)

if(MSVC)
    target_compile_options(hardest_c_bug PRIVATE /W4)
else()
    target_compile_options(hardest_c_bug PRIVATE -Wall -Wextra -Wpedantic)
endif()

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)
if(ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(hardest_c_bug PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(hardest_c_bug PRIVATE -fsanitize=address,undefined)
endif()
