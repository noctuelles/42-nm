/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 16:48:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/06 16:53:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf.h"
#include "unity.h"
#include "utils.h"

void
test_uint32_t_BE_to_host_byte_order() {
#if BYTE_ORDER == BIG_ENDIAN
    TEST_ASSERT_EQUAL_UINT32(0x01EFCDABu, uint32_t_BE_to_host_byte_order(0x01EFCDABu));
#elif BYTE_ORDER == LITTLE_ENDIAN
    TEST_ASSERT_EQUAL_UINT32(0x01EFCDABu, uint32_t_BE_to_host_byte_order(0x01EFCDABu));
#endif
}

int
main() {
    UNITY_BEGIN();
    RUN_TEST(test_uint32_t_BE_to_host_byte_order);
    return (UNITY_END());
}