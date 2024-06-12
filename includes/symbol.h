/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbol.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 16:56:41 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/12 11:23:52 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <elf.h>

#include "libft.h"
#include "parse_elf.h"

typedef struct s_symbol {
    t_elf_parsed_symbol elf_parsed_symbol;
    t_elf_parsed_shdr   elf_parsed_related_shdr;
} t_symbol;

t_list *new_symbol_elem();

#endif