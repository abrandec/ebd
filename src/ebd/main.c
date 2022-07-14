#include "disasm.h"

#include "../common/assets/assets.h"
#include "../common/cmd/argparse/argparse.h"
#include "../common/io/io.h"
#include "../common/utils/hex_utils/hex_utils.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

// To get the compiler to not complain about formating for our custom integer
// types
#pragma GCC diagnostic ignored "-Wformat"

// argparse
static const char *const usages[] = {
    vevm_logo,
    NULL,
};

// load bytecode from input
// @param input: input bytecode
void load_bytecode_input(char *input, char *filename) {
  int file_size = strlen(input);
  disasm(input, filename);
}

// load bytecode from file
// @param file: file to load bytecode from
void load_bytecode_file(char *file, char *filename) {
  FILE *fd;

  long file_size;

  file = read_file_fmmap(fd, file, &file_size);

  file == NULL ? exit(1) : 0;
  disasm(file, filename);
  safe_munmap(file, file_size);
}

// main program
int main(int argc, const char *argv[]) {

  // to hold input and file chars from argparse
  char *file = NULL;
  char *input = NULL;
  char *filename = NULL;

  // param variables
  int debug = 0;
  int perms = 0;

  struct argparse_option options[] = {
      OPT_GROUP("Commands"),
      OPT_STRING('i', "input", &input, "input bytecode"),
      OPT_STRING('f', "file", &file, "file with bytecode", NULL, 0, 0),
      OPT_STRING('o', "output", &filename,
                  "output file location (default terminal output)", NULL, 0, 0),
      OPT_HELP(),
      OPT_END(),
  };

  // parse arguments
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argc = argparse_parse(&argparse, argc, argv);

  // ┌───────────────────┐
  // │   INPUT BYTECODE  │
  // └───────────────────┘
  if (input != NULL) {
    load_bytecode_input(input, filename);
  }

  // ┌───────────────────────────┐
  // │   READ FILE WITH BYTCODE  │
  // └───────────────────────────┘
  if (file != NULL) {
    load_bytecode_file(file, filename);
  }

  // ┌───────────────────────────────────┐
  // │   IF NO COMMAND EXISTS            │
  // └───────────────────────────────────┘
  if (argv != 0 && file == NULL && input == NULL) {
    argparse_usage(&argparse);
  }

  return 0;
}