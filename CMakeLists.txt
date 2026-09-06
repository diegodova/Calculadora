cmake_minimum_required(VERSION 3.20)

project(Calculadora LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Calculadora WIN32
    Calculadora.cpp
)

target_compile_definitions(Calculadora PRIVATE
    UNICODE
    _UNICODE
)

target_link_libraries(Calculadora PRIVATE
    user32
    gdi32
)
