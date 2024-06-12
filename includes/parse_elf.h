/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 14:16:31 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/12 14:53:02 by plouvel          ###   ########.fr       */
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

    ELF_PARSE_INVALID_SYMBOL_SHNDX,
    /* Header related errors */
    ELF_PARSE_INVALID_HDR_ENTRY_SIZE,
    ELF_PARSE_INVALID_HDR_NO_SHDRS,
} t_elf_parse_error;

typedef struct s_elf_parsed_hdr {
    uint8_t  ei_magic[4];
    uint8_t  ei_endianess;
    uint8_t  ei_class;
    size_t   shdr_tab_off;
    uint16_t shdr_tab_size;
    uint16_t shdr_tab_ent_size;
    uint16_t shdr_tab_ent_nbr;
} t_elf_parsed_hdr;

typedef struct s_elf_parsed_shdr {
    size_t offset;
    size_t size;
    size_t ent_size;
    size_t link_val;
    size_t flags;
    size_t type;
} t_elf_parsed_shdr;

typedef struct s_elf_parsed_symbol {
    size_t   value;
    size_t   size;
    uint32_t name;
    uint16_t shndx;
    uint8_t  info;
    uint8_t  other;
} t_elf_parsed_symbol;

t_elf_parsed_hdr  parse_elf_hdr(const void *elf_hdr);
t_elf_parse_error check_elf_hdr(const t_file *file, const t_elf_parsed_hdr *hdr);

t_elf_parsed_shdr parse_elf_shdr(const void *shdr, const t_elf_parsed_hdr *hdr);
t_elf_parse_error check_elf_shdr_symtab(const t_file *file, const t_elf_parsed_shdr *shdr, const t_elf_parsed_hdr *hdr);
t_elf_parse_error check_elf_shdr_strtab(const t_file *file, const t_elf_parsed_shdr *shdr);

t_list *parse_elf_symbols(const t_file *file);

#endif