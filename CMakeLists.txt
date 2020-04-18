cmake_minimum_required(VERSION 3.12)
project(kgfx) 

enable_testing()
add_subdirectory(external/klib) 
add_subdirectory(src/${PROJECT_NAME}) 
