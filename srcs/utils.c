/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:17:15 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/07 12:28:23 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>

#define SWAP_UINT16(x) (((x) >> 8U) | ((x) << 8U))
#define SWAP_UINT32(x) (((x) >> 24U) | (((x) >> 8U) & 0x0000FF00U) | (((x) << 8U) & 0x00FF0000U) | ((x) << 24U))
#define SWAP_UINT64(x)                                                                            \
    (((x) >> 56U) | (((x) >> 40U) & 0x000000000000FF00U) | (((x) >> 24U) & 0x0000000000FF0000U) | \
     (((x) >> 8U) & 0x00000000FF000000U) | (((x) << 8U) & 0x000000FF00000000U) |                  \
     (((x) << 24U) & 0x0000FF0000000000U) | (((x) << 40U) & 0x00FF000000000000U) | ((x) << 56U))

#include "conf.h"

uint16_t
uint16_t_BE_to_host_byte_order(uint16_t value) {
#if BYTE_ORDER == BIG_ENDIAN
    return (value);
#elif BYTE_ORDER == LITTLE_ENDIAN
    return (SWAP_UINT16(value));
#else
#error "Unsupported byte order"
#endif
}

uint16_t
uint16_t_LE_to_host_byte_order(uint16_t value) {
#if BYTE_ORDER == BIG_ENDIAN
    return (SWAP_UINT16(value));
#elif BYTE_ORDER == LITTLE_ENDIAN
    return (value);
#else
#error "Unsupported byte order"
#endif
}

uint32_t
uint32_t_BE_to_host_byte_order(uint32_t value) {
#if BYTE_ORDER == BIG_ENDIAN
    return (value);
#elif BYTE_ORDER == LITTLE_ENDIAN
    return (SWAP_UINT32(value));
#else
#error "Unsupported byte order"
#endif
}

uint32_t
uint32_t_LE_to_host_byte_order(uint32_t value) {
#if BYTE_ORDER == BIG_ENDIAN
    return (SWAP_UINT32(value));
#elif BYTE_ORDER == LITTLE_ENDIAN
    return (value);
#else
#error "Unsupported byte order"
#endif
}

uint64_t
uint64_t_BE_to_host_byte_order(uint64_t value) {
#if BYTE_ORDER == BIG_ENDIAN
    return (value);
#elif BYTE_ORDER == LITTLE_ENDIAN
    return (SWAP_UINT64(value));
#else
#error "Unsupported byte order"
#endif
}

uint64_t
uint64_t_LE_to_host_byte_order(uint64_t value) {
#if BYTE_ORDER == BIG_ENDIAN
    return (SWAP_UINT64(value));
#elif BYTE_ORDER == LITTLE_ENDIAN
    return (value);
#else
#error "Unsupported byte order"
#endif
}
