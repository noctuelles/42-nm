/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbol.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 16:56:41 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/14 11:24:26 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <elf.h>

#include "libft.h"
#include "parse_elf.h"

typedef struct s_sym {
    const char       *name; /* symbol name */
    char              type;
    t_elf_parsed_sym  elf_sym;      /* elf file values */
    t_elf_parsed_shdr elf_rel_shdr; /* related section header */
} t_sym;

typedef struct s_syms_info {
    uint8_t           ei_class;
    t_elf_parsed_shdr shdr_symtab;
    t_elf_parsed_shdr shdr_strtab;
    t_elf_parsed_shdr shdr_shstrtab;
    t_list           *sym_list;
} t_syms_info;

t_elf_parse_error resolve_syms_name(const t_file *file, t_syms_info *syms_info);

void print_syms(const t_syms_info *syms_info);

#endif