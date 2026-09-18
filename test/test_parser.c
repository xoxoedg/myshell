#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "src/parser.h"
#include "test.h"

void test_split_pipe_basic(void) {
    char test_string[] = "ls | grep test";
    int count;

    char **arr = split_pipes(test_string, &count);

    TEST_ASSERT(count == 2);
    TEST_ASSERT(strcmp(arr[0], "ls") == 0);
    TEST_ASSERT(strcmp(arr[1], "grep test") == 0);

    free(arr);
}

void test_split_pipe_multiple(void) {
    char test_string[] = "ls | grep | test | wc";
    int count;

    char **arr = split_pipes(test_string, &count);

    TEST_ASSERT(count == 4);
    TEST_ASSERT(strcmp(arr[0], "ls") == 0);
    TEST_ASSERT(strcmp(arr[1], "grep") == 0);
    TEST_ASSERT(strcmp(arr[2], "test") == 0);
    TEST_ASSERT(strcmp(arr[3], "wc") == 0);

    free(arr);
}

void test_split_pipe_no_pipe(void) {
    char test_string[] = "ls";
    int count;

    char **arr = split_pipes(test_string, &count);

    TEST_ASSERT(count == 1);
    TEST_ASSERT(strcmp(arr[0], "ls") == 0);

    free(arr);
}

int main(void) {
    test_split_pipe_basic();
    test_split_pipe_multiple();
    test_split_pipe_no_pipe();

    TEST_SUMMARY();
    return failures != 0;
}
