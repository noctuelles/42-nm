/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbol.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 16:56:41 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/22 19:05:52 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <elf.h>

#include "libft.h"
#include "parse_elf.h"

typedef struct s_sym {
    const char       *name;         /* symbol's name */
    const char       *rel_sec_name; /* symbols's related section name */
    t_elf_parsed_sym  elf_sym;      /* elf file values */
    t_elf_parsed_shdr elf_rel_shdr; /* related section header */
    bool              has_rel_sec;  /* true if the symbol has a related section */
} t_sym;

typedef struct s_syms_info {
    t_elf_parsed_hdr  hdr;           /* ELF header */
    t_elf_parsed_shdr shdr_symtab;   /* Section header of the symbol table */
    t_elf_parsed_shdr shdr_strtab;   /* Section header of the string table */
    t_elf_parsed_shdr shdr_shstrtab; /* Section header of the section header string table */
    t_list           *sym_list;      /* List of symbols */
} t_syms_info;

void print_syms(const t_syms_info *syms_info);
int  sort_sym(const void *a, const void *b);
int  sort_sym_rev(const void *a, const void *b);

#endif