/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 14:16:31 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/07 15:53:50 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_ELF_H
#define PARSING_ELF_H

#include "elf_utils.h"
#include "file.h"

typedef enum e_elf_parse_error {
    ELF_PARSE_OK,
    ELF_PARSE_FILE_TOO_SHORT,
    ELF_PARSE_NO_SHDR_TABLE,
    ELF_PARSE_UNRECOGNIZED_FORMAT,
} t_elf_parse_error;

typedef struct s_elf_parsing_context {
    t_elf_class class;
    t_elf_endianess endianess;

    const uint8_t *shdr_table;
    size_t         shdr_table_entry_size;
    size_t         shdr_table_nbr;
} t_elf_parsing_context;

const char       *elf_parse_error_to_string(t_elf_parse_error error);
t_elf_parse_error parse_elf_hdr_ident(const t_file *file, t_elf_parsing_context *context);
t_elf_parse_error parse_elf_hdr_shdr(const t_file *file, t_elf_parsing_context *context);

#endif