#ifndef ERRORS_H
#define ERRORS_H

#include <inttypes.h>

// 
#define BYTECODE_MAX_SIZE 0x01
#define BYTECODE_MIN_SIZE 0x02
#define BYTECODE_INVALID 0x03

// IO Errors
#define IO_FILENAME_NULL 0x20
#define IO_DATA_NULL 0x21
#define IO_FILE_OPEN_FAIL 0x22
#define IO_FILE_CREATION_ERR 0x23
#define IO_WRITE_ACCESS_ERR 0x24
#define IO_FOLDER_NAME_ALREADY_EXISTS 0x25
#define IO_FOLDER_NAME_TOO_LONG 0x26
/*
  ┌───────────────────────────────┐
  │   FUNCTIONS                   │
  └───────────────────────────────┘
 */

// print error and exit
// @param err_code: error code
void print_error(uint8_t err_code);

#endif