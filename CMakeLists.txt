cmake_minimum_required(VERSION 3.16)
project(physnum_ex1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Visualiser graphiquement
set(SDL_VISUALIZE 0)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")
add_executable(${PROJECT_NAME} Exercice1_student.cpp)
if(${SDL_VISUALIZE})
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_VISUALIZE=1)
    find_package(SDL3 REQUIRED)
    target_sources(${PROJECT_NAME} PRIVATE SDL_render.cpp)
    target_link_libraries(${PROJECT_NAME} SDL3::SDL3)
else()
    target_link_libraries(${PROJECT_NAME})
endif()

