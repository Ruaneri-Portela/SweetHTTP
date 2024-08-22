cmake_minimum_required(VERSION 3.0)
project(SweetHTTP)

# Define as flags de compilação
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -g")

# Define o diretório de saída
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Diretórios de origem
set(SOURCE_DIR src)
set(SWEETSOCKET_DIR ${CMAKE_SOURCE_DIR}/SweetSocket)
set(SWEETSOCKET_BUILD_DIR ${SWEETSOCKET_DIR}/build)
set(SWEETSOCKET_SRC_DIR ${SWEETSOCKET_DIR}/src)

# Encontrar todos os arquivos fonte
file(GLOB_RECURSE SRC_FILES ${SOURCE_DIR}/*.c)

# Cria a biblioteca compartilhada ou estática do SweetSocket
if(WIN32)
    set(SWEETSOCKET_LIB_NAME "SweetSocket.dll")
    set(SWEETSOCKET_LIB_TYPE SHARED)
else()
    set(SWEETSOCKET_LIB_NAME "libSweetSocket.so")
    set(SWEETSOCKET_LIB_TYPE SHARED)
endif()

add_library(SweetSocket ${SWEETSOCKET_LIB_TYPE}
    IMPORTED)
set_target_properties(SweetSocket PROPERTIES
    IMPORTED_LOCATION ${SWEETSOCKET_BUILD_DIR}/${SWEETSOCKET_LIB_NAME}
)

# Adiciona a biblioteca SweetSocket
add_subdirectory(${SWEETSOCKET_DIR})

# Cria o executável
add_executable(SweetHTTP ${SRC_FILES})

# Adiciona links e diretórios
if(WIN32)
    target_link_libraries(SweetHTTP SweetSocket)
else()
    target_link_libraries(SweetHTTP SweetSocket)
endif()

include_directories(${SOURCE_DIR} ${SWEETSOCKET_SRC_DIR})

# Definindo o diretório de execução para o executável
if(WIN32)
    set(EXE_SUFFIX ".exe")
else()
    set(EXE_SUFFIX "")
endif()

set_target_properties(SweetHTTP PROPERTIES
    OUTPUT_NAME "SweetHTTP"
    SUFFIX ${EXE_SUFFIX}
)

# Adiciona um alvo de limpeza
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
)

# Comando para limpar
set(CLEAN_COMMAND "rm -rf ${CMAKE_BINARY_DIR}")