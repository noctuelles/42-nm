/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_elf.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:05:22 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/07 23:04:25 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing_elf.h"

#include <assert.h>

#include "file.h"
#include "libft.h"
#include "utils.h"

const char *
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

/**
 * @brief Parse the ELF header identification bytes.
 *
 * @param file File to parse.
 * @param context Result-argument to store the parsing context of the ELF file such as class and endianess.
 * @return t_elf_parse_error ELF_PARSE_OK if the file is a valid ELF file, ELF_PARSE_FILE_TOO_SHORT if the file is too
 * short to contain the ELF header, ELF_PARSE_UNRECOGNIZED_FORMAT if the file is not an ELF file.
 */
t_elf_parse_error
parse_elf_hdr_ident(const t_file *file, t_elf_parsing_context *context) {
    const Elf32_Ehdr *header  = NULL;
    const uint8_t     magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

    header = try_read_file(file, 0, sizeof(*header));
    if (header == NULL) {
        return (ELF_PARSE_FILE_TOO_SHORT);
    }
    if (ft_memcmp(header->e_ident, magic, sizeof(magic)) != 0) {
        return (ELF_PARSE_UNRECOGNIZED_FORMAT);
    }
    context->class = header->e_ident[EI_CLASS];
    if (context->class != ELFCLASS32 && context->class != ELFCLASS64) {
        return (ELF_PARSE_UNRECOGNIZED_FORMAT);
    }
    context->endianess = header->e_ident[EI_DATA];
    if (context->endianess != ELFDATA2LSB && context->endianess != ELFDATA2MSB) {
        return (ELF_PARSE_UNRECOGNIZED_FORMAT);
    }
    return (ELF_PARSE_OK);
}

t_elf_parse_error
parse_elf_hdr_shdr(const t_file *file, t_elf_parsing_context *context) {
    assert(context->class != ELF_CLASS_UNDEF);
    assert(context->endianess != ELF_ENDIAN_UNDEF);

    const void *elf_hdr         = NULL;
    uint16_t    shdr_offset     = 0;
    uint16_t    shdr_nbr        = 0;
    uint16_t    shdr_entry_size = 0;

    elf_hdr = try_read_file(file, 0, context->class == ELF_CLASS_32 ? sizeof(Elf32_Ehdr) : sizeof(Elf64_Ehdr));
    if (elf_hdr == NULL) {
        return (ELF_PARSE_FILE_TOO_SHORT);
    }

    if (context->class == ELF_CLASS_32) {
        shdr_offset     = ((const Elf32_Ehdr *)elf_hdr)->e_shoff;
        shdr_nbr        = ((const Elf32_Ehdr *)elf_hdr)->e_shnum;
        shdr_entry_size = ((const Elf32_Ehdr *)elf_hdr)->e_shentsize;
    } else {
        shdr_offset     = ((const Elf64_Ehdr *)elf_hdr)->e_shoff;
        shdr_nbr        = ((const Elf64_Ehdr *)elf_hdr)->e_shnum;
        shdr_entry_size = ((const Elf64_Ehdr *)elf_hdr)->e_shentsize;
    }
    if (shdr_nbr == 0) {
        return (ELF_PARSE_NO_SHDR_TABLE);
    }

    if (context->endianess == ELF_ENDIAN_LITTLE) {
        shdr_offset     = uint16_t_LE_to_host_byte_order(shdr_offset);
        shdr_nbr        = uint16_t_LE_to_host_byte_order(shdr_nbr);
        shdr_entry_size = uint16_t_LE_to_host_byte_order(shdr_entry_size);
    } else {
        shdr_offset     = uint16_t_BE_to_host_byte_order(shdr_offset);
        shdr_nbr        = uint16_t_BE_to_host_byte_order(shdr_nbr);
        shdr_entry_size = uint16_t_BE_to_host_byte_order(shdr_entry_size);
    }

    context->shdr_entry_size = shdr_entry_size;
    context->shdr_nbr        = shdr_nbr;
    context->shdr_start      = try_read_file(file, shdr_offset, shdr_offset + (shdr_nbr * shdr_entry_size));
    if (context->shdr_start == NULL) {
        return (ELF_PARSE_FILE_TOO_SHORT);
    }

    return (ELF_PARSE_OK);
}

t_elf_parse_error
parse_elf_shdr_symtab(const t_file *file, t_elf_parsing_context *context) {
    size_t         i           = 0;
    const uint8_t *section_ptr = NULL;
    (void)file;

    // const uint8_t *symtab_ptr           = NULL;
    // const uint8_t *symtab_str_table_ptr = NULL;

    // size_t symtab_size = 0;

    while (i < context->shdr_nbr) {
        section_ptr = context->shdr_start + (i * context->shdr_entry_size);
        if (((const Elf32_Shdr *)section_ptr)->sh_type == SHT_SYMTAB) {
        }
        i++;
    }
    return (ELF_PARSE_OK);
}

// t_elf_parse_error
// parse_elf_shdr_symbols(const t_file *file, const t_elf_parsing_context *context) {
//     const uint8_t *shdr_table = NULL;
//     size_t         n          = 0;

//     assert(context->class != ELF_CLASS_UNDEF);
//     assert(context->endianess != ELF_ENDIAN_UNDEF);
//     assert(context->shdr_table != NULL);

//     while (n < context->shdr_table_nbr) {
//         shdr_table = context->shdr_table + (n * context->shdr_table_entry_size);
//         n++;
//     }
// }