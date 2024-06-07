/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 13:24:18 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/07 14:02:40 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "file.h"
#include "unity.h"

typedef struct s_file {
    uint8_t *buf_start;
    uint8_t *buf_end;
} t_file;

void
setUp() {}

void
tearDown() {}

void
test_load_file() {
    t_file *file  = load_file("assets/tests/random_file_1024");
    t_file *file2 = load_file("assets/tests/random_file_32768");

    TEST_ASSERT_EQUAL_UINT64(1024, file->buf_end - file->buf_start);
    TEST_ASSERT_EQUAL_UINT64(32768, file2->buf_end - file2->buf_start);

    free_file(file);
    free_file(file2);
}

void
test_load_file_DIRECTORY() {
    t_file *file = load_file("assets/tests");

    TEST_ASSERT_NULL(file);
}

void
test_try_read_file() {
    t_file *file = load_file("assets/tests/random_file_1024");

    TEST_ASSERT_NOT_NULL(file);

    TEST_ASSERT_EQUAL_PTR(file->buf_start, try_read_file(file, 0, 1024));
    TEST_ASSERT_EQUAL_PTR(file->buf_start + 511, try_read_file(file, 511, 512));
    TEST_ASSERT_EQUAL_PTR(file->buf_start + 1023, try_read_file(file, 1023, 1024));

    TEST_ASSERT_EQUAL_PTR(NULL, try_read_file(file, 1024, 1));
    TEST_ASSERT_EQUAL_PTR(NULL, try_read_file(file, 1024, 1024));
    TEST_ASSERT_EQUAL_PTR(NULL, try_read_file(file, 512, 512));
    TEST_ASSERT_EQUAL_PTR(NULL, try_read_file(file, 0, 1025));

    free_file(file);
}

int
main() {
    UNITY_BEGIN();
    RUN_TEST(test_load_file);
    RUN_TEST(test_load_file_DIRECTORY);
    RUN_TEST(test_try_read_file);
    return (UNITY_END());
}