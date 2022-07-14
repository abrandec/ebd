#include "opcodes_t.h"

#include "../../src/opcodes/op_names.h"
#include "../test_utils/assert.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const static char incorrect_value[20] = "incorrect value\0";

// assert that opcode chars match the length of static sizes
// @return: true if all opcodes are correct length, false otherwise
bool assert_len_opcodes(void) {
    bool truthy_truth = true;
    for (int i = 0; i < 256; ++i) {
        // account for strlen not being able to handle nulls
        op_names[i] == NULL ? ++i : assert_eq_wonk(&truthy_truth, strlen(op_names[i]), op_char_len[i]);
     
        truthy_truth ? 0: assert_eq_msg_loop(__func__, incorrect_value, i, strlen(op_names[i]), op_char_len[i]);
    }

    return assert_msg(__func__, truthy_truth);
}

bool opcode_tests(void) {
    enum { test_len = 1 };

    // run all tests
    bool test_results[test_len] = { assert_len_opcodes()};

  return assert_bool_array_msg(__func__, test_results, test_len);
}
