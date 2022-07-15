#include "errors.h"
#include "../common/cmd/color_codes.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

void print_error(uint8_t err_code) {
  switch (err_code) {
  // ┌───────────────────┐
  // │   DISASM ERRORS   │
  // └───────────────────┘
  case BYTECODE_MAX_SIZE:
    printf(YELLOW "Bytecode max size!\n" RESET);
  case BYTECODE_MIN_SIZE:
    printf(YELLOW "Bytecode min size!\n" RESET);
  case BYTECODE_INVALID:
    printf(YELLOW "Bytecode invalid!\n" RESET);
  // ┌───────────────────┐
  // │   IO ERRORS       │
  // └───────────────────┘
  case IO_FILENAME_NULL:
    printf(YELLOW"Filename is NULL!\n"RESET);
    break;
  case IO_DATA_NULL:
    printf(YELLOW"Data is NULL!\n"RESET);
    break;
  case IO_FILE_OPEN_FAIL:
    printf(YELLOW"Error opening file!\n"RESET);
    break;
  case IO_FILE_CREATION_ERR:
    printf(YELLOW"Error creating file!\n"RESET);
    break;
  case IO_WRITE_ACCESS_ERR:
    printf(YELLOW"No access to parent directory!  Try running vevm with sudo.\n"RESET);
    break;
  case IO_FOLDER_NAME_ALREADY_EXISTS:
    printf(YELLOW"Folder name already exists!\n"RESET);
    break;
  case IO_FOLDER_NAME_TOO_LONG:
    printf(YELLOW"Folder name is too long!\n"RESET);
    break;
  default:
    printf("Unknown error!\n"RESET);
    break;
  }
  exit(1);
}