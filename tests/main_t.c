// tests
#include "opcodes_t/opcodes_t.h"

#include "../src/common/cmd/argparse/argparse.h"
#include "../src/common/utils/hex_utils/hex_utils.h"
#include "test_utils/assert.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const static char vevm_logo[48] =
    "\033[7m\e[0;37mevm bytecode disassembler tests\033[00m\0";

// argparse
static const char *const usages[] = {
    vevm_logo,
    NULL,
};

int main(int argc, const char *argv[]) {

  // param variables
  int _all_tests = 0;
  int _opcode_tests = 0;


  struct argparse_option options[] = {
      OPT_GROUP("Commands"),
      OPT_BOOLEAN('a', "all", &_all_tests, "Run all tests", NULL, 0, 0),
      OPT_BOOLEAN('o', "opcode", &_opcode_tests, "Run opcode tests", NULL, 0, 0),
      OPT_HELP(),
      OPT_END(),
  };

  // parse arguments
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argc = argparse_parse(&argparse, argc, argv);

  // ┌───────────────────┐
  // │   ALL TESTS       │
  // └───────────────────┘
  if (_all_tests != 0) {
    enum { test_len = 1 };
    bool test_results[test_len] = { opcode_tests() };
    assert_bool_array_msg("FINAL RESULTS", test_results, test_len);
  }

  // ┌───────────────────┐
  // │   OPCODES TESTS   │
  // └───────────────────┘
  if (_opcode_tests != 0) {
    enum { test_len = 1 };
    bool test_results[test_len] = { opcode_tests() };
    assert_bool_array_msg("FINAL RESULTS", test_results, test_len);
  }

  // ┌───────────────────────────────────┐
  // │   IF NO COMMAND EXISTS            │
  // └───────────────────────────────────┘
  if (argv != 0 && _all_tests == 0 && _opcode_tests == 0) {
    argparse_usage(&argparse);
  }

  return 0;
}
