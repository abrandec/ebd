#ifndef DSM_H
#define DSM_H

#include <stdbool.h>

// EVM bytecode disassembler
// @param bytes: bytecode to disassemble
// @param filename: filename to write disassembly to
// @param output: true if output should be written to stdout, false if output should be written to file
void disasm(char *bytes, char *filename, bool output);

#endif