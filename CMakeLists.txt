cmake_minimum_required(VERSION 3.16)
project(SHIT LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Avisos do compilador (MSVC usa /W4, GCC/Clang usam -Wall -Wextra)
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

add_executable(shit main.c sha1.c)
target_include_directories(shit PRIVATE .)
