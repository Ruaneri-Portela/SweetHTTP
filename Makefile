# Definições de variáveis
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g
LDFLAGS =

out_dir = build
src_dir = src

# Lista de arquivos fonte e objetos
src = $(wildcard $(src_dir)/*.c)
obj = $(addprefix $(out_dir)/, $(notdir $(src:.c=.o)))

lib = SweetSocket
sweet_socket_dir = $(lib)
sweet_socket_build = ../$(sweet_socket_dir)/build
sweet_socket_src = ../$(sweet_socket_dir)/src

ifeq ($(OS),Windows_NT)
    exec = .exe
    lib_ext = .dll
else
    exec =
    lib_ext = .so
endif

TARGET = $(out_dir)/SweetHTTP$(exec)

# Alvo principal
all: genVersion $(TARGET)

genVersion: utils/genVersion.sh
	./$<

# Criação do diretório de saída
$(out_dir):
	mkdir -p $@

# Alvo para criar e copiar a biblioteca
$(out_dir)/$(lib)$(lib_ext): $(sweet_socket_build)/$(lib)$(lib_ext)
	cp $< $@

# Compila a biblioteca SweetSocket
$(sweet_socket_build)/$(lib)$(lib_ext):
	$(MAKE) -C $(sweet_socket_dir)

# Compila arquivos objeto
$(out_dir)/%.o: $(src_dir)/%.c | $(out_dir) $(sweet_socket_build)/$(lib)$(lib_ext)
	$(CC) $(CFLAGS) -c -o $@ $< -I$(sweet_socket_src)

# Cria o executável
$(TARGET): $(obj) $(out_dir)/$(lib)$(lib_ext)
	$(CC) $(CFLAGS) -o $@ $^ -I$(sweet_socket_src) -L$(out_dir) -l$(lib) $(LDFLAGS)

# Limpeza dos arquivos gerados
clean:
	rm -rf $(out_dir)
	$(MAKE) -C $(sweet_socket_dir) clean

.PHONY: all clean