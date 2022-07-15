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
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wformat"

// argparse
static const char *const usages[] = {
    vevm_logo,
    NULL,
};

// load bytecode from input
// @param input: input bytecode
// @param output: output name to write assembly to
void load_bytecode_input(char *input, char *output) { disasm(input, output); }

// load bytecode from file
// @param file: file to load bytecode from
// @param filename: filename of file to load bytecode from
void load_bytecode_file(char *file, char *output) {
  FILE *fd;

  file = read_file_fmmap(fd, file);
  
  disasm(file, output);
}

// main program
int main(int argc, const char *argv[]) {

  // to hold input and file chars from argparse
  char *input = NULL;
  char *file = NULL;
  char *output = NULL;

  struct argparse_option options[] = {
      OPT_GROUP("Commands"),
      OPT_STRING('i', "input", &input, "input bytecode"),
      OPT_STRING('f', "file", &file, "file with bytecode", NULL, 0, 0),
      OPT_STRING('o', "output", &output,
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
  if (file == NULL) {
    load_bytecode_input(input, output);
  }

  // ┌───────────────────────────┐
  // │   READ FILE WITH BYTCODE  │
  // └───────────────────────────┘
  if (file != NULL) {
    load_bytecode_file(file, output);
  }

  // ┌───────────────────────────────────┐
  // │   IF NO COMMAND EXISTS            │
  // └───────────────────────────────────┘
  if (argv != 0 && file == NULL && input == NULL) {
    argparse_usage(&argparse);
  }

  return 0;
}