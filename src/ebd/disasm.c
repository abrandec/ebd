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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wswitch-bool"

struct parse_data {
  // file descriptor for mmap
  int fd;
  // buffers
  char *input;
  char *output;
  // optional output filename
  char *output_name;
  // offsets
  off_t input_offset;
  off_t output_offset;
  // misc
  int opcode;
  int total_opcodes;
  int total_output_size;
};

typedef struct parse_data parse_data_t;

// read push op for offset stuff
// @param parse_data: struct for parsing bytecode
// @param counter: program counter for reading the bytecode
// @param is_push: if true, return the push opcode, otherwise increment
// total_size and counter
// @return: the push opcode if is_push is true, otherwise 0
int read_push(parse_data_t *parse_data, int *counter, bool is_push) {
  switch (parse_data->opcode) {
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
      *counter += parse_data->opcode - 0x5F;
      // account for ' ', '0', 'x'
      parse_data->total_output_size += ((parse_data->opcode - 0x5F) * 2) + 3;
      return 0;
    case 1:
      return parse_data->opcode - 0x5F;
    }
  default:
    return 0;
  }
}

// should also be better at handling invalid bytecode
// check if bytecode is valid & calculate size of output buffer
// @param parse_data: struct for parsing bytecode
void validate_bytecode(parse_data_t *parse_data) {
  int bytecode_size = strlen(parse_data->input);
  int bytecode_len = bytecode_size / 2;
  // validate bytecode size by checking if its even or odd
  bytecode_size % 2 ? print_error(BYTECODE_INVALID) : 0;

  int i = 0;
  for (; i < bytecode_len; ++i) {
    parse_data->opcode = hex_char2int(parse_data->input, 2 * i);
    read_push(parse_data, &i, false);
    i >= bytecode_len ? print_error(BYTECODE_INVALID) : 0;
    ++parse_data->total_opcodes;
    parse_data->total_output_size += op_char_len[parse_data->opcode];
  }

  // for spacing & null byte
  parse_data->total_output_size += parse_data->total_opcodes + 1;
  parse_data->total_opcodes = parse_data->total_opcodes;
}

// parse bytecode into mneumonics
// @param parse_data: struct for parsing bytecode
void parser(parse_data_t *parse_data) {

  // just passing in random pointers to use this function, only opcode &&
  // is_push are used
  int push_val = read_push(parse_data, &parse_data->input_offset, true);

  if (push_val != 0) {
    for (int i = 0; i < op_char_len[parse_data->opcode]; ++i) {
      parse_data->output[parse_data->output_offset] =
          op_names[parse_data->opcode][i];
      ++parse_data->output_offset;
    }

    parse_data->output[parse_data->output_offset] = ' ';
    ++parse_data->output_offset;
    parse_data->output[parse_data->output_offset] = '0';
    ++parse_data->output_offset;
    parse_data->output[parse_data->output_offset] = 'x';
    ++parse_data->output_offset;

    // num bytes to eat from push opcode
    int nom = push_val * 2;
    // increment offset to consume bytes
    parse_data->input_offset += 2;

    int i = 0;
    for (; i < nom; ++i) {
      parse_data->output[parse_data->output_offset] =
          parse_data->input[parse_data->input_offset + i];
      ++parse_data->output_offset;
    }
    parse_data->output[parse_data->output_offset] = '\n';
    ++parse_data->output_offset;
    parse_data->input_offset += nom;
  } else {
    int i = 0;
    for (; i < op_char_len[parse_data->opcode]; ++i) {
      parse_data->output[parse_data->output_offset] =
          op_names[parse_data->opcode][i];
      ++parse_data->output_offset;
    }

    parse_data->output[parse_data->output_offset] = '\n';
    ++parse_data->output_offset;
    parse_data->input_offset += 2;
  }
}

// for looping through the bytecode and parsing it
// @param parse_data: struct for parsing bytecode
void loopty_loop(parse_data_t *parse_data) {

  int i = 0;
  for (; i < parse_data->total_opcodes; ++i) {
    parse_data->opcode =
        hex_char2int(parse_data->input, parse_data->input_offset);
    parser(parse_data);
  }
}

// allocate memory for the output_name buffer using mmap
// @param parse_data: struct for parsing bytecode
void _mmap_alloc(parse_data_t *parse_data) {
  parse_data->output =
      mmap(0, parse_data->total_output_size, PROT_READ | PROT_WRITE,
           MAP_PRIVATE | MAP_ANON, parse_data->fd, 0);
  if (parse_data->output_name == MAP_FAILED) {
    perror("Error mmapping the file");
    exit(EXIT_FAILURE);
  }

  loopty_loop(parse_data);

  write(STDOUT_FILENO, parse_data->output, parse_data->total_output_size);

  // Don't forget to free the mmapped memory
  if (munmap(parse_data->output, parse_data->total_output_size) == -1) {
    perror("Error un-mmapping the file");
    exit(EXIT_FAILURE);
  }
}

// create file from bytecode
// @param parse_data: struct for parsing bytecode
void _create_file(parse_data_t *parse_data) {
  parse_data->fd =
      open(parse_data->output_name, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

  if (write(parse_data->fd, "", parse_data->total_output_size - 1) == -1) {
    close(parse_data->fd);
    perror("Error writing last byte of the file");
    exit(EXIT_FAILURE);
  }

  // update mmap size to match file size to stop bus error
  ftruncate(parse_data->fd, parse_data->total_output_size);

  parse_data->output =
      mmap(0, parse_data->total_output_size, PROT_READ | PROT_WRITE, MAP_SHARED,
           parse_data->fd, 0);

  if (parse_data->output == MAP_FAILED) {
    close(parse_data->fd);
    perror("Error mmapping the file");
    exit(EXIT_FAILURE);
  }

  loopty_loop(parse_data);

  // Write it now to disk
  if (msync(parse_data->output, parse_data->total_output_size, MS_SYNC) == -1) {
    perror("Could not sync the file to disk");
  }

  // Don't forget to free the mmapped memory
  if (munmap(parse_data->output, parse_data->total_output_size) == -1) {
    close(parse_data->fd);
    perror("Error un-mmapping the file");
    exit(EXIT_FAILURE);
  }

  // Un-mmaping doesn't close the file, so we still need to do that.
  close(parse_data->fd);
}

void disasm(char *bytes, char *output_name) {
  // check if bytes is null and if bytes exceeds 3073 bytes (max bytecode len +
  // '\0')
  strlen(bytes) > 87785 ? print_error(BYTECODE_MAX_SIZE) : 0;

  parse_data_t parse_data;

  // init pointers
  parse_data.input = bytes;
  parse_data.output_name = output_name;

  // init data for parsing bytecode
  // set to -1 for _mmap_alloc, _create_file will change it to the output file
  parse_data.fd = -1;
  parse_data.total_opcodes = 0;
  parse_data.input_offset = 0;
  parse_data.output_offset = 0;
  parse_data.total_output_size = 0;

  validate_bytecode(&parse_data);

  parse_data.output_name == NULL ? _mmap_alloc(&parse_data)
                                 : _create_file(&parse_data);
}
