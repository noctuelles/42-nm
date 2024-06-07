/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 16:48:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/07 13:23:21 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "conf.h"
#include "libft.h"
#include "unity.h"
#include "utils.h"

void
tearDown() {}

void
setUp() {}

void
test_uint16_t_BE_to_host_byte_order() {
#if BYTE_ORDER == BIG_ENDIAN
    TEST_ASSERT_EQUAL_UINT16(0xABCDu, uint16_t_BE_to_host_byte_order(0xABCDu));
#elif BYTE_ORDER == LITTLE_ENDIAN
    TEST_ASSERT_EQUAL_HEX16(0xCDABu, uint16_t_BE_to_host_byte_order(0xABCDu));
#endif
}

void
test_uint16_t_LE_to_host_byte_order() {
#if BYTE_ORDER == BIG_ENDIAN
    TEST_ASSERT_EQUAL_HEX16(0xCDABu, uint16_t_LE_to_host_byte_order(0xABCDu));
#elif BYTE_ORDER == LITTLE_ENDIAN
    TEST_ASSERT_EQUAL_UINT16(0xABCDu, uint16_t_LE_to_host_byte_order(0xABCDu));
#endif
}

void
test_uint32_t_BE_to_host_byte_order() {
#if BYTE_ORDER == BIG_ENDIAN
    TEST_ASSERT_EQUAL_UINT32(0xABCDEF01u, uint32_t_BE_to_host_byte_order(0xABCDEF01u));
#elif BYTE_ORDER == LITTLE_ENDIAN
    TEST_ASSERT_EQUAL_HEX32(0x01EFCDABu, uint32_t_BE_to_host_byte_order(0xABCDEF01u));
#endif
}

void
test_uint32_t_LE_to_host_byte_order() {
#if BYTE_ORDER == BIG_ENDIAN
    TEST_ASSERT_EQUAL_HEX32(0x01EFCDABu, uint32_t_LE_to_host_byte_order(0xABCDEF01u));
#elif BYTE_ORDER == LITTLE_ENDIAN
    TEST_ASSERT_EQUAL_UINT32(0xABCDEF01u, uint32_t_LE_to_host_byte_order(0xABCDEF01u));
#endif
}

void
test_uint64_t_BE_to_host_byte_order() {
#if BYTE_ORDER == BIG_ENDIAN
    TEST_ASSERT_EQUAL_UINT64(0xABCDEF0123456789u, uint64_t_BE_to_host_byte_order(0xABCDEF0123456789u));
#elif BYTE_ORDER == LITTLE_ENDIAN
    TEST_ASSERT_EQUAL_HEX64(0x8967452301EFCDABu, uint64_t_BE_to_host_byte_order(0xABCDEF0123456789u));
#endif
}

void
test_uint64_t_LE_to_host_byte_order() {
#if BYTE_ORDER == BIG_ENDIAN
    TEST_ASSERT_EQUAL_HEX64(0x8967452301EFCDABu, uint64_t_LE_to_host_byte_order(0xABCDEF0123456789u));
#elif BYTE_ORDER == LITTLE_ENDIAN
    TEST_ASSERT_EQUAL_UINT64(0xABCDEF0123456789u, uint64_t_LE_to_host_byte_order(0xABCDEF0123456789u));
#endif
}

int
main() {
    char str[1 << 10] = {0};

    snprintf(str, sizeof(str), "Byte Order : %s",
             BYTE_ORDER == LITTLE_ENDIAN ? "Little Endian"
             : BYTE_ORDER == BIG_ENDIAN  ? "Big Endian"
                                         : "Unknown");

    UNITY_BEGIN();
    TEST_MESSAGE(str);
    RUN_TEST(test_uint16_t_BE_to_host_byte_order);
    RUN_TEST(test_uint16_t_LE_to_host_byte_order);
    RUN_TEST(test_uint32_t_BE_to_host_byte_order);
    RUN_TEST(test_uint32_t_LE_to_host_byte_order);
    RUN_TEST(test_uint64_t_BE_to_host_byte_order);
    RUN_TEST(test_uint64_t_LE_to_host_byte_order);
    return (UNITY_END());
}