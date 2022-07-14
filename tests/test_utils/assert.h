#ifndef ASSERT_H
#define ASSERT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

// print a message with pass/fail status
// @param msg[]: the message to print
// @param condition: the condition of the test
// @return the passed or failed condition
bool assert_msg(const char msg[], bool condition);

// print a message with pass/fail status
// @param msg[]: the message to print
// @param condition: the condition of the test
// @return the passed or failed condition
bool assert_msg_verbose(char msg[], bool condition);

// assert that two uint64_ts are equal
// @param a: the first uint64_t to compare
// @param b: the second uint64_t to compare
// @return true if a == b, false otherwise
bool assert_eq(uint64_t a, uint64_t b);

// wonky version of assert eq
// @param result: the result of the test
// @param a: the first uint64_t to compare
// @param b: the second uint64_t to compare
void assert_eq_wonk(bool *result, uint64_t a, uint64_t b);

// assert that two uint64_ts are equal with a message
// @param func[]: the function name
// @param msg[]: the message to print
// @param expected: the expected value to compare
// @param actual: the actual value to compare
// @return true if expected == actual, false otherwise
bool assert_eq_msg(const char func[], const char msg[], uint64_t expected, uint64_t actual);

// assert that two uint64_ts are equal with a message
// @param func[]: the function name
// @param msg[]: the message to print
// @param index: the index of the test
// @param expected: the expected value to compare
// @param actual: the actual value to compare
// @return true if expected == actual, false otherwise
bool assert_eq_msg_loop(const char func[], const char msg[], int index, uint64_t expected,
                   uint64_t actual);

// assert that two strings are equal
// @param func[]: the function name
// @param msg[]: the message to print
// @param expected: the expected string to compare
// @param actual: the actual string to compare
// @return true if expected == actual, false otherwise
bool assert_char(const char func[], const char msg[], char *expected, char *actual);

// uint64_t pseudo number generator
// @return a pseudo random number
uint64_t rand_num(void);

// assert that all the values in an array of true
// @param msg[]: the message to print
// @param arr[]: the array to check
// @param len: the length of the array
// @return true if all the values in the array are true, false otherwise
bool assert_bool_array_msg(const char msg[], bool arr[], size_t len);

#endif