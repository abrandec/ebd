#ifndef ERRORS_H
#define ERRORS_H

#include <inttypes.h>

// Stack Errors
#define STACK_OVERFLOW 0xA0
#define STACK_UNDERFLOW 0xA1
#define STACK_INVALID_INDEX 0xA2
#define STACK_NULL 0xA3

// VM Errors
#define VM_INVALID_OPCODE 0xB0
#define VM_INVALID_ARGUMENT 0xB1
#define VM_MEMORY_SIZE_EXCEEDED 0xB2
#define STOP_INSTRUCTION 0xB3

// IO Errors
#define IO_FILENAME_NULL 0xE0
#define IO_DATA_NULL 0xE1
#define IO_FILE_OPEN_FAIL 0xE2
#define IO_FILE_CREATION_ERR 0xE3
#define IO_WRITE_ACCESS_ERR 0xE4
#define IO_FOLDER_NAME_ALREADY_EXISTS 0xE5
#define IO_FOLDER_NAME_TOO_LONG 0xE6

/*
  ┌───────────────────────────────┐
  │   FUNCTIONS                   │
  └───────────────────────────────┘
 */

// custom error handling
// @param err_code: error code
void custom_error(uint8_t err_code);

#endif