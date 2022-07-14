
#include "assert.h"

#include "../../src/common/cmd/color_codes.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>

#pragma GCC diagnostic ignored "-Wformat"

bool assert_msg(const char msg[], bool condition) {
  if (condition) {
    printf(GREEN "│ ▸ %s - PASS\n" RESET, msg);
    return true;
  } else {
    printf(RED "│ ▸ %s - FAIL\n" RESET, msg);
    return false;
  }
}

bool assert_eq(uint64_t a, uint64_t b) { return (a == b); }

void assert_eq_wonk(bool *result, uint64_t a, uint64_t b) { *result = (a == b); }

bool assert_eq_msg(const char func[], const char msg[], uint64_t expected,
                   uint64_t actual) {
  if (expected == actual) {
    printf(GREEN "│ ▸ %s - pass\n" RESET, func);
    return true;
  } else {
    printf(RED "│   \n│ ▸ %s - fail\n│   %s\n│   ├─ expected: %020lld\n│ "
               "  └─ actual:   %020lld\n" RESET,
           func, msg, expected, actual);
    return false;
  }
}

bool assert_eq_msg_loop(const char func[], const char msg[], int index, uint64_t expected,
                   uint64_t actual) {
  if (expected == actual) {
    printf(GREEN "│ ▸ %s - pass\n" RESET, func);
    return true;
  } else {
    printf(RED "│   \n│ ▸ %s - fail\n│   %s\n│   index: %d\n│   ├─ expected: %020lld\n│ "
               "  └─ actual:   %020lld\n" RESET,
           func, msg, index, expected, actual);
    return false;
  }
}

bool assert_char(const char func[], const char msg[], char *expected,
                 char *actual) {
  if (strcmp(expected, actual) == 0) {
    printf(GREEN "│ ▸ %s - pass\n" RESET, func);
    return true;
  } else {
    printf(RED "│   \n│ ▸ %s - fail\n│   %s\n│   ├─ expected: %s\n│ "
               "  └─ actual:   %s\n" RESET,
           func, msg, expected, actual);
    return false;
  }
}

uint64_t rand_num(void) {
  unsigned int tmp;

  getrandom(&tmp, sizeof(unsigned int), GRND_NONBLOCK) == -1
      ? perror("getrandom")
      : "";

  return tmp;
}

bool assert_bool_array_msg(const char msg[], bool arr[], size_t len) {
  int i = 0;

  for (; i < len; ++i) {
    if (!arr[i]) {
      printf(RED "├──────────────────────────────────\n│ %s - "
                 "FAIL\n├──────────────────────────────────\n" RESET,
             msg);

      return false;
    }
  }

  printf(GREEN "├──────────────────────────────────\n│ %s - "
               "PASS\n├──────────────────────────────────\n" RESET,
         msg);

  return true;
}
