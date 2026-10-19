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

file (GLOB SOURCES "*.c")
add_executable(shit ${SOURCES})
target_include_directories(shit PRIVATE .)

# Instalacao (usada por install.sh / install.ps1):
#   cmake --install build --prefix <pasta>
install(TARGETS shit RUNTIME DESTINATION .)

# Testes de linha de comando (execute com: ctest --test-dir build --output-on-failure)
enable_testing()
add_test(
  NAME cli
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli.sh $<TARGET_FILE:shit>
)
