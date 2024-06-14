/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 16:49:18 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/14 13:48:12 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define IS_SET(flags, mask) (((flags) & (mask)) == (mask))

uint16_t uint16_t_BE_to_host_byte_order(uint16_t value);
uint16_t uint16_t_LE_to_host_byte_order(uint16_t value);
uint32_t uint32_t_BE_to_host_byte_order(uint32_t value);
uint32_t uint32_t_LE_to_host_byte_order(uint32_t value);
uint64_t uint64_t_BE_to_host_byte_order(uint64_t value);
uint64_t uint64_t_LE_to_host_byte_order(uint64_t value);

#endif