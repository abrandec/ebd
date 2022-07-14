# Vision EVM
# Author: Abran DeCarlo
# LICENSE: AGPL-3.0-ONLY

# Compiler Options
CC := clang
LIBS := -lm
FLAGS := -Oz

UTILS := src/errors/errors.c src/common/utils/hex_utils/hex_utils.c src/common/cmd/argparse/argparse.c src/common/io/io.c src/ebd/disasm.c

TESTS := tests/opcodes_t/opcodes_t.c tests/test_utils/assert.c

#  											
#						Compiling					
#													

all: ebd

tests: main_t	

#								 
#		  Sample Programs		
#								 

# EBD (Ethereum Bytecode Disassembler)
ebd: src/ebd/main.c 
	$(CC) -o bin/ebd/ebd $(UTILS) src/ebd/main.c $(LIBS)

#   					
# 			Tests 		
#							

# Main Test Suite
main_t: tests/main_t.c
	$(CC) $(UTILS) -o bin/tests/main_t $(VM) tests/main_t.c $(TESTS) -DTEST $(FLAGS) $(LIBS)