/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbol.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:06:57 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/13 15:19:49 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "symbol.h"

#include <stdio.h>
#include <stdlib.h>

#include "libft.h"

/**
 * @brief  Check if the symbol name is correctly terminated.
 *
 * @param strtab String table section header containing the symbol name.
 * @param ptr Pointer to the symbol name.
 * @return t_elf_parse_error ELF_PARSE_OK if the symbol name is correctly, otherwise ELF_PARSE_CORRUPT_STRTABLE.
 */
static t_elf_parse_error
check_syms_name(const t_elf_parsed_shdr *strtab, const char *ptr) {
    size_t i = 0;

    while (i < strtab->size) {
        if (ptr[i] == '\0') {
            return (ELF_PARSE_OK);
        }
        i++;
    }
    return (ELF_PARSE_CORRUPT_STRTABLE);
}

t_elf_parse_error
resolve_syms_name(const t_file *file, t_syms_info *syms_info) {
    t_elf_parse_error        ret_val    = ELF_PARSE_OK;
    t_list                  *elem       = NULL;
    t_sym                   *sym        = NULL;
    size_t                   str_offset = 0;
    const t_elf_parsed_shdr *str_tab    = NULL;

    elem = syms_info->sym_list;
    while (elem != NULL) {
        sym = elem->content;
        if (sym->elf_sym.type == STT_SECTION) {
            str_offset = syms_info->shdr_shstrtab.offset + sym->elf_rel_shdr.name;
            str_tab    = &syms_info->shdr_shstrtab;
        } else {
            str_offset = syms_info->shdr_strtab.offset + sym->elf_sym.name;
            str_tab    = &syms_info->shdr_strtab;
        }
        sym->name = get_file_ptr_from_offset(file, str_offset);
        if ((ret_val = check_syms_name(str_tab, sym->name)) != ELF_PARSE_OK) {
            return (ret_val);
        }
        puts(sym->name);
        elem = elem->next;
    }
    return (ret_val);
}
