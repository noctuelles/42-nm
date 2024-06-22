/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elf_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 11:47:56 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/22 19:29:01 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "file.h"
#include "parse_elf.h"

t_elf_parse_error
check_elf_name(const t_file *file, const t_elf_parsed_shdr *strtab, uint32_t name, t_elf_parse_error ret_err) {
    const char *strtab_name = NULL;
    size_t      rem_bytes   = 0;

    if (!is_file_ptr_offset_valid(file, strtab->offset + name)) {
        return (ret_err);
    }
    if (name >= strtab->size) {
        return (ret_err);
    }
    strtab_name = get_file_ptr_from_offset(file, strtab->offset + name);
    rem_bytes   = strtab->size - name;
    while (*strtab_name != '\0') {
        if (rem_bytes == 0) {
            return (ret_err);
        }
        strtab_name++;
        rem_bytes--;
    }
    return (ELF_PARSE_OK);
}