cmake_minimum_required(VERSION 3.8)
project(mpiProjects)

set(CMAKE_CXX_STANDARD 11)

set(TASK1_SOURCE task1.cpp)
add_executable(task1 ${TASK1_SOURCE})

set(TASK2_SOURCE task2.cpp)
add_executable(task2 ${TASK2_SOURCE})