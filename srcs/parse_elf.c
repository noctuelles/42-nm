/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:05:22 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/17 12:19:21 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse_elf.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "libft.h"
#include "symbol.h"
#include "utils.h"

/**
 * @brief Translate an ELF parsing error to a human-readable string.
 *
 * @param error Error to translate.
 * @return const char* Human-readable string.
 */
static const char *
elf_parse_error_to_string(t_elf_parse_error error) {
    switch (error) {
        case ELF_PARSE_OK:
            return ("sucess");
        case ELF_PARSE_FILE_TOO_SHORT:
            return ("file too short");
        case ELF_PARSE_UNRECOGNIZED_FORMAT:
            return ("file format not recognized");
            /* Related to symbol parsing */
        case ELF_PARSE_INVALID_SYMBOL_SHNDX:
            return ("invalid symbol section index");
            /* Related to header parsing */
        case ELF_PARSE_INVALID_HDR_CLASS:
            return ("invalid elf header class");
        case ELF_PARSE_INVALID_HDR_ENDIANESS:
            return ("invalid elf header endianess");
        case ELF_PARSE_INVALID_HDR_MAGIC:
            return ("invalid elf header magic");
        case ELF_PARSE_INVALID_HDR_SHDR_ENTRY_NBR:
            return ("invalid elf header section header entry number");
        case ELF_PARSE_INVALID_HDR_SHDR_ENTRY_SIZE:
            return ("invalid elf header section header entry size");
        case ELF_PARSE_INVALID_HDR_SHDRS:
            return ("invalid elf header section headers (wrong size or wrong offset)");
        case ELF_PARSE_INVALID_HDR_SHDR_STRNDX:
            return ("invalid elf header section header string table index");
        case ELF_PARSE_CORRUPT_STRTABLE:
            return ("corrupted string table");
        case ELF_PARSE_INTERNAL_ERROR:
            return ("internal error");
        default:
            return ("unknown error");
    }
}

static t_elf_parse_error
iter_symtab(const t_file *file, const t_elf_parsed_hdr *hdr, t_syms_info *syms_info) {
    t_elf_parse_error ret_val  = ELF_PARSE_OK;
    t_sym            *sym      = NULL;
    t_list           *sym_elem = NULL;
    size_t            n        = syms_info->shdr_symtab.ent_size;

    while (n < syms_info->shdr_symtab.size) {
        sym = ft_calloc(sizeof(*sym), 1);
        if (sym == NULL) {
            return (ELF_PARSE_INTERNAL_ERROR);
        }
        sym->elf_sym = parse_elf_sym(get_file_ptr_from_offset(file, syms_info->shdr_symtab.offset + n), hdr);
        if ((ret_val = check_elf_sym(&sym->elf_sym, hdr)) != ELF_PARSE_OK) {
            free(sym);
            return (ret_val);
        }
        if (!SHN_RESERVED(sym->elf_sym.shndx)) {
            sym->elf_rel_shdr =
                parse_elf_shdr(get_file_ptr_from_offset(file, hdr->shdr_tab_off + (sym->elf_sym.shndx * hdr->shdr_tab_ent_size)), hdr);
        }
        sym_elem = ft_lstnew(sym);
        if (sym_elem == NULL) {
            free(sym);
            return (ELF_PARSE_INTERNAL_ERROR);
        }
        ft_lstadd_back(&syms_info->sym_list, sym_elem);
        n += syms_info->shdr_symtab.ent_size;
    }

    return (ret_val);
}

static t_elf_parse_error
iter_shdrs(const t_file *file, const t_elf_parsed_hdr *hdr, t_syms_info *syms_info) {
    const uint8_t    *shdr_ptr        = NULL;
    const uint8_t    *shdr_strtab_ptr = NULL;
    size_t            i               = 1;
    t_elf_parse_error ret_val         = ELF_PARSE_OK;

    shdr_ptr = get_file_ptr_from_offset(file, hdr->shdr_tab_off);
    while (i < hdr->shdr_tab_ent_nbr) {
        if (((const Elf32_Shdr *)shdr_ptr)->sh_type == SHT_SYMTAB) {
            syms_info->shdr_symtab = parse_elf_shdr(shdr_ptr, hdr);
            if ((ret_val = check_elf_shdr_symtab(file, &syms_info->shdr_symtab, hdr)) != ELF_PARSE_OK) {
                return (ret_val);
            }
            shdr_strtab_ptr =
                get_file_ptr_from_offset(file, hdr->shdr_tab_off + (syms_info->shdr_symtab.link_val * hdr->shdr_tab_ent_size));
            syms_info->shdr_strtab = parse_elf_shdr(shdr_strtab_ptr, hdr);
            if ((ret_val = check_elf_shdr_strtab(file, &syms_info->shdr_strtab)) != ELF_PARSE_OK) {
                return (ret_val);
            }
            if ((ret_val = iter_symtab(file, hdr, syms_info)) != ELF_PARSE_OK) {
                return (ret_val);
            }
        }
        shdr_ptr += hdr->shdr_tab_ent_size;
        i++;
    }
    return (ret_val);
}

/**
 * @brief Parse the symbols of an ELF file.
 *
 * @param file File to parse.
 * @return t_list* List of symbols, in the order where they appear in the list (unsorted by any means).
 */
t_list *
parse_elf_symbols(const t_file *file) {
    const void       *elf_hdr   = NULL;
    t_syms_info       syms_info = {0};
    t_elf_parsed_hdr  hdr       = {0};
    t_elf_parse_error ret_val   = ELF_PARSE_FILE_TOO_SHORT;

    errno = 0;
    if ((elf_hdr = try_read_file(file, 0, sizeof(Elf64_Ehdr))) == NULL) {
        if ((elf_hdr = try_read_file(file, 0, sizeof(Elf32_Ehdr))) == NULL) {
            goto err;
        }
    }
    hdr = parse_elf_hdr(elf_hdr);
    if ((ret_val = check_elf_hdr(file, &hdr)) != ELF_PARSE_OK) {
        goto err;
    }
    syms_info.ei_class = hdr.ei_class;
    syms_info.shdr_shstrtab =
        parse_elf_shdr(get_file_ptr_from_offset(file, hdr.shdr_tab_off + (hdr.shdr_tab_strndx * hdr.shdr_tab_ent_size)), &hdr);
    if ((ret_val = check_elf_shdr_strtab(file, &syms_info.shdr_shstrtab)) != ELF_PARSE_OK) {
        goto err;
    }
    if ((ret_val = iter_shdrs(file, &hdr, &syms_info)) != ELF_PARSE_OK) {
        goto err;
    }
    if ((ret_val = resolve_names(file, &syms_info)) != ELF_PARSE_OK) {
        goto err;
    }
    print_syms(&syms_info);
    return (NULL);
err:
    if (syms_info.sym_list != NULL) {
        ft_lstclear(&syms_info.sym_list, NULL);
    }
    ft_error(0, errno, elf_parse_error_to_string(ret_val));
    return (NULL);
}