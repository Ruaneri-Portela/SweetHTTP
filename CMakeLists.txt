cmake_minimum_required(VERSION 3.10)
project(SweetHTTP)

# Define a versão mínima do CMake
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Diretórios
set(OUT_DIR ${CMAKE_BINARY_DIR}/build)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SWEETSOCKET_DIR ${CMAKE_SOURCE_DIR}/SweetSocket)
set(SWEETSOCKET_LIB ${SWEETSOCKET_DIR}/build/SweetSocket.a)

# Cria o diretório de build se não existir
file(MAKE_DIRECTORY ${OUT_DIR})

# Adiciona o diretório de fontes
file(GLOB SRC_FILES ${SRC_DIR}/*.c)

# Adiciona os arquivos de origem
add_executable(SweetHTTP ${SRC_FILES})

# Inclui diretórios
include_directories(${SWEETSOCKET_DIR}/src)

# Linka com a biblioteca SweetSocket
target_link_libraries(SweetHTTP ${SWEETSOCKET_LIB})
