/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:05:22 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/12 18:28:11 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse_elf.h"

#include <assert.h>
#include <stdio.h>

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
        case ELF_PARSE_NO_SHDR_TABLE:
            return ("no section header table");
        case ELF_PARSE_UNRECOGNIZED_FORMAT:
            return ("file format not recognized");
        default:
            return ("unknown error");
    }
}

t_elf_parse_error
iter_symtab(const t_file *file, const t_elf_parsed_shdr *symtab_shdr, const t_elf_parsed_shdr *strtab_shdr, const t_elf_parsed_hdr *hdr) {
    const size_t      ent_nbr = symtab_shdr->size / symtab_shdr->ent_size;
    const uint8_t    *sym_ent = NULL;
    size_t            i       = 0;
    t_elf_parsed_sym  sym     = {0};
    t_elf_parse_error ret_val = ELF_PARSE_OK;

    sym_ent = get_file_ptr_from_offset(file, symtab_shdr->offset);
    while (i < ent_nbr) {
        sym = parse_elf_sym(sym_ent, hdr);
        if ((ret_val = check_elf_sym(file, &sym, strtab_shdr)) != ELF_PARSE_OK) {
            return (ret_val);
        }
        printf("\"%s\", name : %u, strndx : %u\n", get_sym_name(file, &sym, strtab_shdr), sym.name, sym.shndx);
        sym_ent += symtab_shdr->ent_size;
        i++;
    }

    return (ret_val);
}

t_elf_parse_error
iter_shdrs(const t_file *file, const t_elf_parsed_hdr *hdr) {
    const uint8_t    *shdr        = NULL;
    const uint8_t    *shdr_strtab = NULL;
    size_t            i           = 1;
    t_elf_parsed_shdr symtab_shdr = {0};
    t_elf_parsed_shdr strtab_shdr = {0};
    t_elf_parse_error ret_val     = ELF_PARSE_OK;

    shdr = get_file_ptr_from_offset(file, hdr->shdr_tab_off);
    while (i < hdr->shdr_tab_ent_nbr) {
        if (((const Elf32_Shdr *)shdr)->sh_type == SHT_SYMTAB) {
            symtab_shdr = parse_elf_shdr(shdr, hdr);
            if ((ret_val = check_elf_shdr_symtab(file, &symtab_shdr, hdr)) != ELF_PARSE_OK) {
                return (ret_val);
            }
            shdr_strtab = get_file_ptr_from_offset(file, hdr->shdr_tab_off + (symtab_shdr.link_val * hdr->shdr_tab_ent_size));
            strtab_shdr = parse_elf_shdr(shdr_strtab, hdr);
            if ((ret_val = check_elf_shdr_strtab(file, &strtab_shdr)) != ELF_PARSE_OK) {
                return (ret_val);
            }

            if ((ret_val = iter_symtab(file, &symtab_shdr, &strtab_shdr, hdr)) != ELF_PARSE_OK) {
                return (ret_val);
            }
        }
        shdr += hdr->shdr_tab_ent_size;
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
    const void       *elf_hdr = NULL;
    t_elf_parsed_hdr  hdr     = {0};
    t_elf_parse_error ret_val = ELF_PARSE_FILE_TOO_SHORT;

    if ((elf_hdr = try_read_file(file, 0, sizeof(Elf64_Ehdr))) == NULL) {
        if ((elf_hdr = try_read_file(file, 0, sizeof(Elf32_Ehdr))) == NULL) {
            goto err;
        }
    }
    hdr = parse_elf_hdr(elf_hdr);
    if ((ret_val = check_elf_hdr(file, &hdr)) != ELF_PARSE_OK) {
        goto err;
    }

    iter_shdrs(file, &hdr);

    return (NULL);
err:
    ft_error(0, 0, elf_parse_error_to_string(ret_val));
    return (NULL);
}