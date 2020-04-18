find_package(Catch2 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)

set(IS_MSVC "$<CXX_COMPILER_ID:MSVC>")
set(IS_GCC "$<CXX_COMPILER_ID:GNU>")

function(enable_compile_options target)
    # Warning level (MSVC)
    target_compile_options(${target} PUBLIC "$<${IS_MSVC}:/W4>")
    target_compile_options(${target} PUBLIC "$<${IS_MSVC}:/bigobj>")

    # Optimizations GCC (Release)
    target_compile_options(${target} PUBLIC "$<$<AND:${IS_GCC},$<CONFIG:RELEASE>>:-O3>")
endfunction(enable_compile_options)

# Library
add_library(${PROJECT_NAME} STATIC  frame_time.cpp 
                                    event_handler.cpp 
                                    opengl/mesh.cpp 
                                    opengl/renderer.cpp 
                                    opengl/shader.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

target_link_libraries(${PROJECT_NAME} PUBLIC    klib
                                                glm 
                                                SDL2::SDL2 
                                                SDL2::SDL2main 
                                                GLEW::GLEW 
                                                OpenGL::GL )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
enable_compile_options(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_FORCE_LEFT_HANDED)

# Executable
#add_executable(kgfx main.cpp)
#target_link_libraries(kgfx ${PROJECT_NAME})
#enable_compile_options(kgfx)

# Test executable
add_executable(kgfxtest main.test.cpp)
target_link_libraries(kgfxtest ${PROJECT_NAME} Catch2::Catch2)
add_test(NAME kgfxtest COMMAND kgfxtest)
