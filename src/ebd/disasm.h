#ifndef DSM_H
#define DSM_H

#include <stdbool.h>

// EVM bytecode disassembler
// @param bytes: bytecode to disassemble
// @param output: output file to write the disassembled bytecode to
void disasm(char *bytes, char *output);

#endif