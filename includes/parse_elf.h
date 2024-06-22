/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 14:16:31 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/22 19:30:22 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_ELF_H
#define PARSING_ELF_H

#include "elf_utils.h"
#include "file.h"
#include "libft.h"

typedef enum e_elf_parse_error {
    ELF_PARSE_OK,

    /* Symbol related errors */
    ELF_PARSE_INVALID_SYMBOL_SHNDX,

    /* Header related errors */
    ELF_PARSE_INVALID_HDR_MAGIC,
    ELF_PARSE_INVALID_HDR_ENDIANESS,
    ELF_PARSE_INVALID_HDR_CLASS,
    ELF_PARSE_INVALID_HDR_SHDR_ENTRY_SIZE,
    ELF_PARSE_INVALID_HDR_SHDR_ENTRY_NBR,
    ELF_PARSE_INVALID_HDR_SHDR_STRNDX,
    ELF_PARSE_INVALID_HDR_SHDRS,

    /* Symbol table */
    ELF_PARSE_INVALID_SYMTAB_ENTRY_SIZE,
    ELF_PARSE_INVALID_SYMTAB_SIZE,
    ELF_PARSE_INVALID_SYMTAB_STRTAB,
    ELF_PARSE_INVALID_SYMTAB_MAPPED_REGION,

    /* Symbols */

    ELF_PARSE_INVALID_SYM_NAME,
    ELF_PARSE_INVALID_SEC_NAME,

    /* String table */

    ELF_PARSE_INVALID_STRTAB_TYPE,
    ELF_PARSE_INVALID_STRTAB_MAPPED_REGION,

    /* All table */
    ELF_PARSE_INVALID_NAME,
    ELF_PARSE_NO_SYMS,
    ELF_PARSE_INTERNAL_ERROR,
} t_elf_parse_error;

typedef struct s_elf_parsed_hdr {
    uint8_t  ei_magic[4];
    uint8_t  ei_endianess;
    uint8_t  ei_class;
    size_t   shdr_tab_off;
    uint16_t shdr_tab_ent_size;
    uint16_t shdr_tab_ent_nbr;
    uint16_t shdr_tab_strndx;
} t_elf_parsed_hdr;

typedef struct s_elf_parsed_shdr {
    size_t   offset;
    size_t   size;
    size_t   ent_size;
    size_t   link_val;
    size_t   flags;
    size_t   type;
    uint32_t name;
} t_elf_parsed_shdr;

typedef struct s_elf_parsed_sym {
    size_t   value;
    size_t   size;
    uint32_t name;
    uint16_t shndx;
    uint8_t  type;
    uint8_t  bind;
    uint8_t  visibility;
} t_elf_parsed_sym;

#define SHN_RESERVED(shndx) ((shndx) >= SHN_LORESERVE && (shndx) < SHN_HIRESERVE)
#define CHK_PARSE(ret_val, expr) (((ret_val) = (expr)) == ELF_PARSE_OK)

t_elf_parsed_hdr  parse_elf_hdr(const void *elf_hdr);
t_elf_parse_error check_elf_hdr(const t_file *file, const t_elf_parsed_hdr *hdr);

t_elf_parsed_shdr parse_elf_shdr(const void *shdr, const t_elf_parsed_hdr *hdr);
t_elf_parse_error check_elf_shdr_symtab(const t_file *file, const t_elf_parsed_shdr *shdr, const t_elf_parsed_hdr *hdr);
t_elf_parse_error check_elf_shdr_strtab(const t_file *file, const t_elf_parsed_shdr *shdr);

t_elf_parsed_sym  parse_elf_sym(const void *symbol, const t_elf_parsed_hdr *hdr);
t_elf_parse_error check_elf_sym(const t_elf_parsed_sym *sym, const t_elf_parsed_hdr *hdr);

t_elf_parse_error check_elf_name(const t_file *file, const t_elf_parsed_shdr *strtab, uint32_t name, t_elf_parse_error ret_err);

int dump_elf_syms(const t_file *file);

#endif