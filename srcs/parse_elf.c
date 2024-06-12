/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:05:22 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/12 14:54:31 by plouvel          ###   ########.fr       */
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

// static t_elf_parse_error
// iter_elf_symtab(const t_file *file, const t_elf_parse_context *context, const t_elf_parsed_shdr *symtab, const t_elf_parsed_shdr *strtab)
// {
//     t_elf_parse_error   ret_val = ELF_PARSE_OK;
//     t_elf_parsed_symbol symbol  = {0};
//     const void         *sym_ptr = NULL;
//     size_t              i       = 1;

//     (void)strtab;

//     while (i * symtab->ent_size < symtab->size) {
//         sym_ptr = symtab->ptr + (i * symtab->ent_size);
//         if ((ret_val = parse_elf_symbol(file, sym_ptr, context, &symbol)) != ELF_PARSE_OK) {
//             return (ret_val);
//         }
//         i++;
//     }

//     return (ELF_PARSE_OK);
// }

// static t_elf_parse_error
// iter_elf_shdrs(const t_file *file, const t_elf_parse_context *context) {
//     t_elf_parse_error ret_val         = ELF_PARSE_OK;
//     t_elf_parsed_shdr symtab          = {0};
//     t_elf_parsed_shdr symtab_strtab   = {0};
//     const void       *shdr_ptr        = NULL;
//     const void       *shdr_strtab_ptr = NULL;
//     size_t            i               = 0;

//     while (i < context->hdr.shdrs_nbr) {
//         shdr_ptr = context->hdr.shdrs + (i * context->hdr.shdrs_ent_size);
//         if (((const Elf32_Shdr *)shdr_ptr)->sh_type == SHT_SYMTAB) {
//             if ((ret_val = parse_elf_shdr(file, shdr_ptr, context, &symtab)) != ELF_PARSE_OK) {
//                 return (ret_val);
//             }
//             if (!(symtab.link_val > 0 && symtab.link_val < context->hdr.shdrs_nbr)) {
//                 return (ELF_PARSE_UNRECOGNIZED_FORMAT);
//             }
//             shdr_strtab_ptr = context->hdr.shdrs + (symtab.link_val * context->hdr.shdrs_ent_size);
//             if ((ret_val = parse_elf_shdr(file, shdr_strtab_ptr, context, &symtab_strtab)) != ELF_PARSE_OK) {
//                 return (ret_val);
//             }
//             iter_elf_symtab(file, context, &symtab, &symtab_strtab);
//         }
//         i++;
//     }
//     return (ret_val);
// }

t_elf_parse_error
iter_symtab(const t_file *file, const t_elf_parsed_shdr *symtab_shdr, const t_elf_parsed_shdr *strtab_shdr, const t_elf_parsed_hdr *hdr) {
    (void)file;
    (void)symtab_shdr;
    (void)strtab_shdr;
    (void)hdr;

    return (ELF_PARSE_OK);
}

t_elf_parse_error
iter_shdrs(const t_file *file, const t_elf_parsed_hdr *hdr) {
    size_t            i           = 1;
    const uint8_t    *shdr        = NULL;
    const uint8_t    *shdr_strtab = NULL;
    t_elf_parsed_shdr symtab_shdr = {0};
    t_elf_parsed_shdr strtab_shdr = {0};
    t_elf_parse_error ret_val     = ELF_PARSE_OK;

    shdr = get_file_ptr_from_offset(file, hdr->shdr_tab_off);
    while (i < hdr->shdr_tab_ent_nbr) {
        shdr += hdr->shdr_tab_ent_size;
        if (((const Elf32_Shdr *)shdr)->sh_type == SHT_SYMTAB) {
            symtab_shdr = parse_elf_shdr(shdr, hdr);
            if ((ret_val = check_elf_shdr_symtab(file, &symtab_shdr, hdr)) != ELF_PARSE_OK) {
                return (ret_val);
            }
            shdr_strtab = get_file_ptr_from_offset(file, hdr->shdr_tab_off + (symtab_shdr.link_val * hdr->shdr_tab_ent_size));
            strtab_shdr = parse_elf_shdr(shdr_strtab, hdr);
            if ((ret_val = check_elf_shdr_strtab(file, &symtab_shdr)) != ELF_PARSE_OK) {
                return (ret_val);
            }

            if ((ret_val = iter_symtab(file, &symtab_shdr, &strtab_shdr, hdr)) != ELF_PARSE_OK) {
                return (ret_val);
            }
        }
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