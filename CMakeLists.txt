cmake_minimum_required(VERSION 3.10)
project(OctreeUTEC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(octree_demo main.cpp)

# Mensajes informativos
message(STATUS "Proyecto: Octree - UTEC 2025")
message(STATUS "Ejecutable: octree_demo")
message(STATUS "Estándar C++: ${CMAKE_CXX_STANDARD}")