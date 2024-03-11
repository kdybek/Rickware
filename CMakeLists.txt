cmake_minimum_required(VERSION 3.27)
project(Rickware)

set(CMAKE_CXX_STANDARD 17)

add_library(RickDLL SHARED RickExecute.cpp)

add_executable(Rickware Rickware.cpp)

add_executable(Victim Victim.cpp)

add_custom_target(BuildAll ALL
        DEPENDS RickDLL Rickware Victim
)

set_target_properties(BuildAll PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(BuildAll PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")