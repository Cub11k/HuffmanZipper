SRC_DIR = src
SRC = huffman.cpp compress.cpp decompress.cpp test.cpp huffman_tree.cpp

HDR_DIR = src
HDR = huffman_tree.h

SCRIPTS_DIR = scripts

SRC_PATH = $(addprefix $(SRC_DIR)/, $(SRC))
HDR_PATH = $(addprefix $(HDR_DIR)/, $(HDR))

GXX = g++
FLAGS = -O2 -Wall -Werror

huffman: $(SRC_PATH) $(HDR_PATH)
	$(GXX) $(SRC_PATH) -o $@ $(FLAGS)
	chmod 755 $(SCRIPTS_DIR)/*

.PHONY: clean

clean:
	rm huffman