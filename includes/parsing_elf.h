/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_elf.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 14:16:31 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/08 13:16:26 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_ELF_H
#define PARSING_ELF_H

#include "elf_utils.h"
#include "file.h"
#include "libft.h"

typedef enum e_elf_parse_error {
    ELF_PARSE_OK,
    ELF_PARSE_FILE_TOO_SHORT,
    ELF_PARSE_NO_SHDR_TABLE,
    ELF_PARSE_UNRECOGNIZED_FORMAT,
} t_elf_parse_error;

typedef struct s_elf_parsed_shdr {
    const uint8_t *sec_ptr;
    size_t         sec_size;
    size_t         sec_ent_size;
    size_t         sec_off;
    size_t         sec_link_val;
} t_elf_parsed_shdr;

typedef struct s_elf_parsing_context {
    t_elf_class class;
    t_elf_endianess endianess;

    const uint8_t *shdr_start;
    size_t         shdr_entry_size;
    size_t         shdr_nbr;

    const uint8_t *curr_section;
    size_t         curr_section_size;
    size_t         curr_section_entry_size;
    size_t         curr_section_offset;
    size_t         curr_section_link_value;

    const uint8_t *curr_section_strtab;

    const uint8_t *curr_symtab_str_table;

} t_elf_parsing_context;

typedef struct s_elf_symbol {
    const char *name;
    union {
        Elf32_Addr _32;
        Elf64_Addr _64;
    } value;
} t_elf_symbol;

t_elf_parse_error parse_elf_hdr_ident(const t_file *file, t_elf_parsing_context *context);
t_elf_parse_error parse_elf_hdr_shdr(const t_file *file, t_elf_parsing_context *context);
t_elf_parse_error parse_elf_shdrs(const t_file *file, t_elf_parsing_context *context);

t_list *parse_elf_symbols(const t_file *file);

#endif