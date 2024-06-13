/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbol.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 16:56:41 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/13 13:09:14 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <elf.h>

#include "libft.h"
#include "parse_elf.h"

typedef struct s_sym {
    t_elf_parsed_sym  elf_sym;
    t_elf_parsed_shdr elf_rel_shdr;
} t_sym;

typedef struct s_syms_info {
    uint8_t           ei_class;
    t_elf_parsed_shdr shdr_symtab;
    t_elf_parsed_shdr shdr_strtab;
    t_elf_parsed_shdr shdr_shstrtab;
    t_list           *sym_list;
} t_syms_info;

t_list *new_symbol_elem();

#endif