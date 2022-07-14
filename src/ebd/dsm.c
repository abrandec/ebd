#include "dsm.h"

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

const static char char_null_err[11] = "char null\0";
const static char bytecode_max_err[34] = "bytecode size exceeds 3072 bytes\0";
const static char bytecode_min_err[34] = "bytecode size is less than 1 byte\0";

void print_exit(const char *msg) {
  printf(YELLOW "%s\n" RESET, msg);
  exit(1);
}

// Parse push opcode
void read_op(int *opcode, int *total_size, int *counter) {
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
    *counter += *opcode - 0x5F;
    // account for whitespace, 0x, \n
    *total_size += ((*opcode - 0x5F) * 2) + 3;
    break;
  default:
    break;
  }
}

// Parse push opcode
void is_push(char *buf, char *bytes, int *opcode, int *buf_offset,
             int *bytes_offset) {

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
    for (int i = 0; i < op_char_len[*opcode]; ++i) {
      buf[*buf_offset] = op_names[*opcode][i];
      ++*buf_offset;
    }
    *bytes_offset += 2;
    buf[*buf_offset] = ' ';
    ++*buf_offset;
    buf[*buf_offset] = '0';
    ++*buf_offset;
    buf[*buf_offset] = 'x';
    ++*buf_offset;

    int nom = (*opcode - 0x5F) * 2;

    for (int i = 0; i < nom; ++i) {
      buf[*buf_offset] = bytes[*bytes_offset + i];
      ++*buf_offset;
    }

    buf[*buf_offset] = '\n';
    ++*buf_offset;
    *bytes_offset += nom;

    break;
  default:
    break;
  }
}

int calc_file_size(char *bytes, int *total_opcodes) {
  int opcode = 0;

  int total_ops = 0;
  int total_file_size = 0;

  int bytecode_len = strlen(bytes) / 2;

  int i = 0;
  for (; i < bytecode_len; ++i) {
    opcode = hex_char2int(bytes, 2 * i);
    read_op(&opcode, &total_file_size, &i);
    ++total_ops;
    total_file_size += op_char_len[opcode];
  }

  // for spacing & null byte
  total_file_size += total_ops + 1;
  *total_opcodes = total_ops;
  return total_file_size;
}

void writer(char *buf, char *bytes, int *opcode, int *buf_offset,
            int *bytes_offset) {
  is_push(buf, bytes, opcode, buf_offset, bytes_offset);
  *opcode = hex_char2int(bytes, *bytes_offset);

  int i = 0;
  for (; i < op_char_len[*opcode]; ++i) {
    buf[*buf_offset] = op_names[*opcode][i];
    ++*buf_offset;
  }

  buf[*buf_offset] = '\n';
  ++*buf_offset;
}

void write2_file(char *buf, char *bytes, int filesize, int *total_opcodes) {
  int opcode = 0;
  int buf_offset = 0;
  int bytes_offset = 0;

  int i = 0;
  for (; i < *total_opcodes; ++i) {
    opcode = hex_char2int(bytes, bytes_offset);

    writer(buf, bytes, &opcode, &buf_offset, &bytes_offset);
    bytes_offset += 2;
  }
}

void create2_file(char *bytes, char *filename, int textsize,
                  int *total_opcodes) {
  if (!filename) {
    custom_error(IO_FILENAME_NULL);
    return;
  }

  int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

  // size_t textsize = strlen(bytes) + 1; // + \0 null character

  if (write(fd, "", textsize - 1) == -1) {
    close(fd);
    perror("Error writing last byte of the file");
    exit(EXIT_FAILURE);
  }

  char *map = mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    close(fd);
    perror("Error mmapping the file");
    exit(EXIT_FAILURE);
  }

  write2_file(map, bytes, textsize, total_opcodes);

  //printf("%s\n", map);

  // Write it now to disk

  if (msync(map, textsize, MS_SYNC) == -1) {
    perror("Could not sync the file to disk");
  }

  // Don't forget to free the mmapped memory
  if (munmap(map, textsize) == -1) {
    close(fd);
    perror("Error un-mmapping the file");
    exit(EXIT_FAILURE);
  }

  // Un-mmaping doesn't close the file, so we still need to do that.
  close(fd);
}

void parse(char *bytes, char *filename) {
  // check if bytes is null and if bytes exceeds 3073 bytes (max bytecode len +
  // '\0')
  bytes == NULL          ? print_exit(char_null_err)
  : strlen(bytes) > 3073 ? print_exit(bytecode_max_err)
                         : 0;

  int total_opcodes = 0;
  int file_size = calc_file_size(bytes, &total_opcodes);

  // check if file size is less than 1 byte or causes segfault
  file_size < 2 ? print_exit(bytecode_min_err) : 0;
  filename == NULL ? print_exit(bytecode_min_err)
                   : create2_file(bytes, filename, file_size, &total_opcodes);
}
