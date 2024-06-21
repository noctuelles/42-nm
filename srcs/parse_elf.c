/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:05:22 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/21 13:58:19 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse_elf.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "ft_nm.h"
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
elf_parse_err_to_str(t_elf_parse_error error) {
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
            /* Symtab Section Header */
        case ELF_PARSE_INVALID_SYMTAB_ENTRY_SIZE:
            return ("invalid symbol table entry size");
        case ELF_PARSE_INVALID_SYMTAB_SIZE:
            return ("invalid symbol table size");
        case ELF_PARSE_INVALID_SYMTAB_STRTAB:
            return ("invalid symbol table string table index");
        case ELF_PARSE_INVALID_SYMTAB_MAPPED_REGION:
            return ("invalid symbol table offset or size");
            /* Strtab Sectio Header */
        case ELF_PARSE_INVALID_STRTAB_TYPE:
            return ("invalid string table type");
        case ELF_PARSE_INVALID_STRTAB_MAPPED_REGION:
            return ("invalid string table offset or size");
            /* All Section Header*/
        case ELF_PARSE_INVALID_SHDR_NAME:
            return ("invalid section header name");
        /* Other */
        case ELF_PARSE_NO_SYMS:
            return ("no symbols");
        case ELF_PARSE_CORRUPT_STRTABLE:
            return ("corrupted string table");
        case ELF_PARSE_INTERNAL_ERROR:
            return ("internal error");
        default:
            return ("unknown error");
    }
}

static t_elf_parse_error
fill_sym_list(const t_file *file, t_syms_info *syms_info) {
    size_t            n        = syms_info->shdr_symtab.ent_size;
    t_elf_parse_error ret_val  = ELF_PARSE_OK;
    t_sym            *sym      = NULL;
    t_list           *sym_elem = NULL;

    while (n < syms_info->shdr_symtab.size) {
        if ((sym = ft_calloc(sizeof(*sym), 1)) == NULL) {
            return (ELF_PARSE_INTERNAL_ERROR);
        }

        sym->elf_sym = parse_elf_sym(get_file_ptr_from_offset(file, syms_info->shdr_symtab.offset + n), &syms_info->hdr);
        if ((ret_val = check_elf_sym(&sym->elf_sym, &syms_info->hdr)) != ELF_PARSE_OK) {
            free(sym);
            return (ret_val);
        }

        /* Resolve related section name, and symbol name */
        if (!SHN_RESERVED(sym->elf_sym.shndx)) {
            sym->elf_rel_shdr = parse_elf_shdr(
                get_file_ptr_from_offset(file, syms_info->hdr.shdr_tab_off + (sym->elf_sym.shndx * syms_info->hdr.shdr_tab_ent_size)),
                &syms_info->hdr);
            if ((ret_val = check_elf_name(file, &syms_info->shdr_shstrtab, sym->elf_rel_shdr.name)) != ELF_PARSE_OK) {
                free(sym);
                return (ret_val);
            }
            sym->rel_sec_name = get_file_ptr_from_offset(file, syms_info->shdr_shstrtab.offset + sym->elf_rel_shdr.name);
        }
        if (sym->elf_sym.type == STT_SECTION) {
            sym->name = sym->rel_sec_name;
        } else {
            if ((ret_val = check_elf_name(file, &syms_info->shdr_strtab, sym->elf_rel_shdr.name)) != ELF_PARSE_OK) {
                free(sym);
                return (ret_val);
            }
            sym->name = get_file_ptr_from_offset(file, syms_info->shdr_strtab.offset + sym->elf_sym.name);
        }

        sym_elem = ft_lstnew(sym);
        if (sym_elem == NULL) {
            free(sym);
            return (ELF_PARSE_INTERNAL_ERROR);
        }
        ft_lstadd_front(&syms_info->sym_list, sym_elem);

        n += syms_info->shdr_symtab.ent_size;
    }
    return (ret_val);
}

static t_elf_parse_error
iter_shdrs(const t_file *file, t_syms_info *syms_info) {
    t_elf_parse_error ret_val         = ELF_PARSE_OK;
    const uint8_t    *shdr_ptr        = NULL;
    const uint8_t    *shdr_strtab_ptr = NULL;
    bool              no_syms         = true;
    size_t            i               = 0;

    while (i < syms_info->hdr.shdr_tab_ent_nbr) {
        shdr_ptr = get_file_ptr_from_offset(file, syms_info->hdr.shdr_tab_off + (i * syms_info->hdr.shdr_tab_ent_size));

        if (((const Elf32_Shdr *)shdr_ptr)->sh_type == SHT_SYMTAB) {
            syms_info->shdr_symtab = parse_elf_shdr(shdr_ptr, &syms_info->hdr);
            if ((ret_val = check_elf_shdr_symtab(file, &syms_info->shdr_symtab, &syms_info->hdr)) != ELF_PARSE_OK) {
                return (ret_val);
            }
            shdr_strtab_ptr = get_file_ptr_from_offset(
                file, syms_info->hdr.shdr_tab_off + (syms_info->shdr_symtab.link_val * syms_info->hdr.shdr_tab_ent_size));
            syms_info->shdr_strtab = parse_elf_shdr(shdr_strtab_ptr, &syms_info->hdr);
            if ((ret_val = check_elf_shdr_strtab(file, &syms_info->shdr_strtab)) != ELF_PARSE_OK) {
                return (ret_val);
            }

            if ((ret_val = fill_sym_list(file, syms_info)) != ELF_PARSE_OK) {
                ft_lstclear(&syms_info->sym_list, free);
                return (ret_val);
            }

            if (!g_opts.no_sort) {
                ft_lstsort(&syms_info->sym_list, g_opts.reverse_sort ? sort_sym_rev : sort_sym);
            }
            no_syms = false;
            print_syms(syms_info);
            ft_lstclear(&syms_info->sym_list, free);
        }
        i++;
    }
    return (no_syms ? ELF_PARSE_NO_SYMS : ELF_PARSE_OK);
}

int
dump_elf_syms(const t_file *file) {
    const void       *elf_hdr   = NULL;
    t_elf_parse_error ret_val   = ELF_PARSE_OK;
    t_syms_info       syms_info = {0};

    errno = 0;
    if ((elf_hdr = try_read_file(file, 0, sizeof(Elf64_Ehdr))) == NULL) {
        if ((elf_hdr = try_read_file(file, 0, sizeof(Elf32_Ehdr))) == NULL) {
            goto err;
        }
    }
    syms_info.hdr = parse_elf_hdr(elf_hdr);
    if ((ret_val = check_elf_hdr(file, &syms_info.hdr)) != ELF_PARSE_OK) {
        goto err;
    }
    syms_info.shdr_shstrtab = parse_elf_shdr(
        get_file_ptr_from_offset(file, syms_info.hdr.shdr_tab_off + (syms_info.hdr.shdr_tab_strndx * syms_info.hdr.shdr_tab_ent_size)),
        &syms_info.hdr);
    if ((ret_val = check_elf_shdr_strtab(file, &syms_info.shdr_shstrtab)) != ELF_PARSE_OK) {
        goto err;
    }

    if ((ret_val = iter_shdrs(file, &syms_info)) != ELF_PARSE_OK) {
        goto err;
    }
    return (0);
err:
    ft_error(0, 0, "'%s': %s", get_file_name(file), elf_parse_err_to_str(ret_val));
    return (1);
}