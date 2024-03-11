cmake_minimum_required(VERSION 3.27)
project(Rickware)

set(CMAKE_CXX_STANDARD 17)

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(FATAL_ERROR "Tough luck, this virus only supports Windows")
endif ()


add_library(RickExecute SHARED RickExecute.cpp)

add_executable(Rickware Rickware.cpp)

add_executable(Victim WIN32 Victim.cpp)

add_custom_target(BuildAll ALL
        DEPENDS RickExecute Rickware Victim
)

set_target_properties(BuildAll PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(BuildAll PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")