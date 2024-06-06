/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbol.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:07:04 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/06 15:33:35 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYMBOL_H
#define SYMBOL_H
#include <stdint.h>

typedef enum e_symbol_type { SYMBOL_TYPE_32, SYMBOL_TYPE_64 } t_symbol_type;

typedef struct s_symbol {
    const char *name;
    union value {
        uint32_t _32;
        uint64_t _64;
    };
} t_symbol;

#endif