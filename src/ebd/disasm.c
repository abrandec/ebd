#include "disasm.h"

#include "../common/cmd/color_codes.h"
#include "../common/io/io.h"
#include "../common/utils/hex_utils/hex_utils.h"
#include "../errors/errors.h"
#include "../opcodes/op_names.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wswitch-bool"

const static char char_null_err[11] = "char null\0";
const static char bytecode_max_err[34] = "bytecode size exceeds 3072 bytes\0";
const static char bytecode_min_err[34] = "bytecode size is less than 1 byte\0";
const static char bytecode_invalid_err[21] = "bytecode is invalid\0";

// print err msg and exit
// @param err_msg: err msg to print
void print_exit(const char *err_msg) {
  printf(YELLOW "%s\n" RESET, err_msg);
  exit(1);
}

// read push op for offset stuff
// @param opcode: opcode to read
// @param total_size: for incrementing size for output
// @param counter: program counter for reading the bytecode
// @param is_push: if true, return the push opcode, otherwise increment
// total_size and counter
// @return: the push opcode if is_push is true, otherwise 0
int read_push(int *opcode, int *total_size, int *counter, bool is_push) {
  switch (*opcode) {
  case 0x60:
  case 0x61:
  case 0x62:
  case 0x63:
  case 0x64:
  case 0x65:
  case 0x66:
  case 0x67:
  case 0x68:
  case 0x69:
  case 0x6A:
  case 0x6B:
  case 0x6C:
  case 0x6D:
  case 0x6E:
  case 0x6F:
  case 0x70:
  case 0x71:
  case 0x72:
  case 0x73:
  case 0x74:
  case 0x75:
  case 0x76:
  case 0x77:
  case 0x78:
  case 0x79:
  case 0x7A:
  case 0x7B:
  case 0x7C:
  case 0x7D:
  case 0x7E:
  case 0x7F:
    switch (is_push) {
    case 0:
      *counter += *opcode - 0x5F;
      // account for ' ', '0', 'x'
      *total_size += ((*opcode - 0x5F) * 2) + 3;
      return 0;
    case 1:
      return *opcode - 0x5F;
    }
  default:
    return 0;
  }
}

// should also be better at handling invalid bytecode
// check if bytecode is valid & calculate size of bytecode output file
// @param bytecode: bytecode
// @param total_opcodes: total number of opcodes in bytecode
// @return: file size in bytes
int validate_bytecode(char *bytecode, int *total_opcodes) {
  int opcode = 0;
  int total_ops = 0;
  int total_file_size = 0;
  int bytecode_size = strlen(bytecode);
  int bytecode_len = bytecode_size / 2;
  // validate bytecode size by checking if its even or odd (really lazy check,
  // ik)
  bytecode_size % 2 ? print_exit(bytecode_invalid_err) : 0;

  int i = 0;
  for (; i < bytecode_len; ++i) {
    opcode = hex_char2int(bytecode, 2 * i);
    read_push(&opcode, &total_file_size, &i, false);
    i >= bytecode_len ? print_exit(bytecode_invalid_err) : 0;
    ++total_ops;
    total_file_size += op_char_len[opcode];
  }

  // for spacing & null byte
  total_file_size += total_ops + 1;
  *total_opcodes = total_ops;
  return total_file_size;
}

// parse bytecode into mneumonics
// @param buf: buffer to store output in
// @param bytecode: bytecode
// @param opcode: for getting the opcode name & size
// @param buf_offset: for incrementing the buffer offset
// @param bytecode_offset: for incrementing the bytecode offset
void parser(char *buf, char *bytes, int *opcode, int *buf_offset,
            int *bytecode_offset) {

  // just passing in random pointers to use this function, only opcode &&
  // is_push are used
  int push_val = read_push(opcode, buf_offset, bytecode_offset, true);

  if (push_val != 0) {
    for (int i = 0; i < op_char_len[*opcode]; ++i) {
      buf[*buf_offset] = op_names[*opcode][i];
      ++*buf_offset;
    }

    buf[*buf_offset] = ' ';
    ++*buf_offset;
    buf[*buf_offset] = '0';
    ++*buf_offset;
    buf[*buf_offset] = 'x';
    ++*buf_offset;

    // num bytes to eat from push opcode
    int nom = push_val * 2;
    // increment offset to consume bytes
    *bytecode_offset += 2;

    for (int i = 0; i < nom; ++i) {
      buf[*buf_offset] = bytes[*bytecode_offset + i];
      ++*buf_offset;
    }

    buf[*buf_offset] = '\n';

    ++*buf_offset;
    *bytecode_offset += nom;

  } else {
    for (int i = 0; i < op_char_len[*opcode]; ++i) {
      buf[*buf_offset] = op_names[*opcode][i];
      ++*buf_offset;
    }

    buf[*buf_offset] = '\n';
    ++*buf_offset;
    *bytecode_offset += 2;
  }
}

// for looping through the bytecode and parsing it
// @param buf: buffer to store output in
// @param filesize: file size in bytes
// @param total_opcodes: total number of opcodes in bytecode
void loopty_loop(char *buf, char *bytes, int filesize, int *total_opcodes) {
  int opcode = 0;
  int buf_offset = 0;
  int bytecode_offset = 0;

  int i = 0;
  for (; i < *total_opcodes; ++i) {
    opcode = hex_char2int(bytes, bytecode_offset);
    parser(buf, bytes, &opcode, &buf_offset, &bytecode_offset);
  }
}

// create temp file for disassembled bytecode
// @param bytecode: bytecode
// @param filename: filename to create
void _temp_file(char *bytes, int filesize, int *total_opcodes) {
  const char *filename = "/tmp/ebd.temp";

  int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

  if (write(fd, "", filesize - 1) == -1) {
    close(fd);
    perror("Error writing last byte of the file");
    exit(EXIT_FAILURE);
  }

  char *map = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    close(fd);
    perror("Error mmapping the file");
    exit(EXIT_FAILURE);
  }

  loopty_loop(map, bytes, filesize, total_opcodes);

  printf("%s\n", map);

  // Don't forget to free the mmapped memory
  if (munmap(map, filesize) == -1) {
    close(fd);
    perror("Error un-mmapping the file");
    exit(EXIT_FAILURE);
  }

  // Un-mmaping doesn't close the file, so we still need to do that.
  close(fd);
}

// create file from bytecode
// @param bytecode: bytecode
// @param filename: filename to create
void _create_file(char *bytes, char *filename, int filesize,
                  int *total_opcodes) {
  if (!filename) {
    custom_error(IO_FILENAME_NULL);
    return;
  }

  int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

  if (write(fd, "", filesize - 1) == -1) {
    close(fd);
    perror("Error writing last byte of the file");
    exit(EXIT_FAILURE);
  }

  char *map = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    close(fd);
    perror("Error mmapping the file");
    exit(EXIT_FAILURE);
  }

  loopty_loop(map, bytes, filesize, total_opcodes);

  // printf("%s\n", map);

  // Write it now to disk
  if (msync(map, filesize, MS_SYNC) == -1) {
    perror("Could not sync the file to disk");
  }

  // Don't forget to free the mmapped memory
  if (munmap(map, filesize) == -1) {
    close(fd);
    perror("Error un-mmapping the file");
    exit(EXIT_FAILURE);
  }

  // Un-mmaping doesn't close the file, so we still need to do that.
  close(fd);
}

void disasm(char *bytes, char *filename, bool output) {
  // check if bytes is null and if bytes exceeds 3073 bytes (max bytecode len +
  // '\0')
  bytes == NULL          ? print_exit(char_null_err)
  : strlen(bytes) > 3073 ? print_exit(bytecode_max_err)
                         : 0;

  int total_opcodes = 0;

  int file_size = validate_bytecode(bytes, &total_opcodes);

  // if output == true then create a file with the disassembled bytecode, else
  // create a temp file to mmap and ouput disassembled bytecode to stdout
  output ? _create_file(bytes, filename, file_size, &total_opcodes)
         : _temp_file(bytes, file_size, &total_opcodes);
}
