#include "errors.h"
#include "../common/cmd/color_codes.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

void custom_error(uint8_t err_code) {
  printf(YELLOW "Error code: %X\n" RESET, err_code);

  switch (err_code) {

  // ┌───────────────────┐
  // │   STACK ERRORS    │
  // └───────────────────┘
  
  case STACK_OVERFLOW:
    printf("└ Stack overflow!\n");
    break;
  case STACK_UNDERFLOW:
    printf("└ Stack underflow!\n");
    break;
  case STACK_INVALID_INDEX:
    printf("└ Stack index invalid!\n");
    break;
  case STACK_NULL:
    printf("└ Stack is NULL!\n");
    break;

  // ┌───────────────────┐
  // │   VM ERRORS       │
  // └───────────────────┘
  case VM_INVALID_OPCODE:
    printf("└ Invalid opcode!\n");
    break;
  case VM_INVALID_ARGUMENT:
    printf("└ Invalid argument!\n");
    break;
  case VM_MEMORY_SIZE_EXCEEDED:
    printf("└ Memory size exceeded!\n");
    break;
  case STOP_INSTRUCTION:
    printf("└ Stop instruction!\n");
    break;

  // ┌───────────────────┐
  // │   IO ERRORS       │
  // └───────────────────┘
  case IO_FILENAME_NULL:
    printf("└ Filename is NULL!\n");
    break;
  case IO_DATA_NULL:
    printf("└ Data is NULL!\n");
    break;
  case IO_FILE_OPEN_FAIL:
    printf("└ Error opening file!\n");
    break;
  case IO_FILE_CREATION_ERR:
    printf("└ Error creating file!\n");
    break;
  case IO_WRITE_ACCESS_ERR:
    printf("└ No access to parent directory!  Try running vevm with sudo.\n");
    break;
  case IO_FOLDER_NAME_ALREADY_EXISTS:
    printf("└ Folder name already exists!\n");
    break;
  case IO_FOLDER_NAME_TOO_LONG:
    printf("└ Folder name is too long!\n");
    break;
  default:
    printf("└ Unknown error!\n");
    break;
  }
}