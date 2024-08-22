CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g
LDFLAGS =

out_dir = build
src_dir = src

src = $(wildcard $(src_dir)/*.c)
obj = $(addprefix $(out_dir)/, $(notdir $(src:.c=.o)))

lib = SweetSocket
sweet_socket_dir = ./$(lib)
sweet_socket_lib_file = $(sweet_socket_dir)/$(lib)
sweet_socket_buid = $(sweet_socket_dir)/build
sweet_socket_src = $(sweet_socket_dir)/src

ifeq ($(OS),Windows_NT)
    exec = .exe
	sweet_socket_lib_file += .dll
else
    exec =
endif

TARGET = $(out_dir)/SweetHTTP$(exec)

all: $(TARGET)

$(out_dir):
	mkdir -p $@

$(sweet_socket_lib_file):
	$(MAKE) -C $(sweet_socket_dir)

$(out_dir)/%.o: $(src_dir)/%.c | $(out_dir) $(sweet_socket_lib_file)
	$(CC) $(CFLAGS) -c -o $@ $< -I$(sweet_socket_src)

$(TARGET): $(obj) 
	$(CC) $(CFLAGS) -o $@ $^ -I$(sweet_socket_src) -L$(sweet_socket_buid) -l$(lib) $(LDFLAGS) 

clean:
	rm -rf $(out_dir)
	$(MAKE) -C $(sweet_socket_dir) clean

.PHONY: all clean