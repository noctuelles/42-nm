/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_elf.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:05:22 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/08 13:25:36 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing_elf.h"

#include <assert.h>

#include "file.h"
#include "libft.h"
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

/**
 * @brief Parse the section header table of an ELF file.
 *
 * @param file File to parse.
 * @param context Result-argument to store the parsing context.
 * @return t_elf_parse_error ELF_PARSE_OK if the section header table was successfully parsed.
 */
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
    if (context->endianess == ELF_ENDIAN_LITTLE) {
        shdr_offset     = uint16_t_LE_to_host_byte_order(shdr_offset);
        shdr_nbr        = uint16_t_LE_to_host_byte_order(shdr_nbr);
        shdr_entry_size = uint16_t_LE_to_host_byte_order(shdr_entry_size);
    } else {
        shdr_offset     = uint16_t_BE_to_host_byte_order(shdr_offset);
        shdr_nbr        = uint16_t_BE_to_host_byte_order(shdr_nbr);
        shdr_entry_size = uint16_t_BE_to_host_byte_order(shdr_entry_size);
    }

    if (shdr_nbr == 0) {
        return (ELF_PARSE_UNRECOGNIZED_FORMAT);
    }
    if (shdr_entry_size < (context->class == ELF_CLASS_32 ? sizeof(Elf32_Shdr) : sizeof(Elf64_Shdr))) {
        return (ELF_PARSE_UNRECOGNIZED_FORMAT);
    }

    context->shdr_entry_size = shdr_entry_size;
    context->shdr_nbr        = shdr_nbr;
    context->shdr_start      = try_read_file(file, shdr_offset, shdr_offset + (shdr_nbr * shdr_entry_size));

    if (context->shdr_start == NULL) {
        return (ELF_PARSE_FILE_TOO_SHORT);
    }

    return (ELF_PARSE_OK);
}

static void
parse_elf_shdr_32(const Elf32_Shdr *shdr, const t_elf_parsing_context *context, t_elf_parsed_shdr *parsed_shdr) {
    Elf32_Off off      = shdr->sh_offset;
    uint32_t  ent_size = shdr->sh_entsize;
    uint32_t  size     = shdr->sh_size;
    uint32_t  link_val = shdr->sh_link;

    if (context->endianess == ELF_ENDIAN_BIG) {
        off      = uint32_t_BE_to_host_byte_order(off);
        ent_size = uint32_t_BE_to_host_byte_order(ent_size);
        size     = uint32_t_BE_to_host_byte_order(size);
        link_val = uint32_t_BE_to_host_byte_order(link_val);
    } else if (context->endianess == ELF_ENDIAN_LITTLE) {
        off      = uint32_t_LE_to_host_byte_order(off);
        ent_size = uint32_t_LE_to_host_byte_order(ent_size);
        size     = uint32_t_LE_to_host_byte_order(size);
        link_val = uint32_t_LE_to_host_byte_order(link_val);
    }

    parsed_shdr->sec_ent_size = ent_size;
    parsed_shdr->sec_size     = size;
    parsed_shdr->sec_off      = off;
    parsed_shdr->sec_link_val = link_val;
}

static void
parse_elf_shdr_64(const Elf64_Shdr *shdr, const t_elf_parsing_context *context, t_elf_parsed_shdr *parsed_shdr) {
    Elf64_Off off      = shdr->sh_offset;
    uint64_t  ent_size = shdr->sh_entsize;
    uint64_t  size     = shdr->sh_size;
    uint64_t  link_val = shdr->sh_link;

    if (context->endianess == ELF_ENDIAN_BIG) {
        off      = uint64_t_BE_to_host_byte_order(off);
        ent_size = uint64_t_BE_to_host_byte_order(ent_size);
        size     = uint64_t_BE_to_host_byte_order(size);
        link_val = uint64_t_BE_to_host_byte_order(link_val);
    } else if (context->endianess == ELF_ENDIAN_LITTLE) {
        off      = uint64_t_LE_to_host_byte_order(off);
        ent_size = uint64_t_LE_to_host_byte_order(ent_size);
        size     = uint64_t_LE_to_host_byte_order(size);
        link_val = uint64_t_LE_to_host_byte_order(link_val);
    }

    parsed_shdr->sec_ent_size = ent_size;
    parsed_shdr->sec_size     = size;
    parsed_shdr->sec_off      = off;
    parsed_shdr->sec_link_val = link_val;
}

/**
 * @brief Dispatcher function for parsing a section header.
 *
 * @param shdr Section header pointer.
 * @param context Context of the parsing.
 * @param parsed_shdr Result-argument storing the parsing result.
 */
static void
parse_elf_shdr(const void *shdr, const t_elf_parsing_context *context, t_elf_parsed_shdr *parsed_shdr) {
    if (context->class == ELF_CLASS_32) {
        parse_elf_shdr_32(shdr, context, parsed_shdr);
    } else if (context->class == ELF_CLASS_64) {
        parse_elf_shdr_64(shdr, context, parsed_shdr);
    }
}

t_elf_parse_error
parse_elf_shdrs(const t_file *file, t_elf_parsing_context *context) {
    t_elf_parsed_shdr sec_symtab              = {0};
    t_elf_parsed_shdr sec_symtab_str_tab      = {0};
    const void       *shdr_ptr                = NULL;
    const void       *symtab_shdr_str_tab_ptr = NULL;
    size_t            i                       = 0;

    (void)file;

    while (i < context->shdr_nbr) {
        shdr_ptr = context->shdr_start + (i * context->shdr_entry_size);
        if (((const Elf32_Shdr *)shdr_ptr)->sh_type == SHT_SYMTAB) {
            parse_elf_shdr(shdr_ptr, context, &sec_symtab);
            // Check for integrity with try_read_file..
            symtab_shdr_str_tab_ptr = context->shdr_start + (sec_symtab.sec_link_val * context->shdr_entry_size);
            parse_elf_shdr(symtab_shdr_str_tab_ptr, context, &sec_symtab_str_tab);
            // Check for integrity with try_read_file..

            // Now we're ready to get the pointer to the symbol table, and a pointer to the symtab string table.
        }
        i++;
    }
    return (ELF_PARSE_OK);
}

t_elf_parse_error
parse_elf_symtab_symbols(const t_file *file, t_elf_parsing_context *context) {
    (void)file;
    (void)context;
    return (ELF_PARSE_OK);
}

/**
 * @brief Parse the symbols of an ELF file.
 *
 * @param file File to parse.
 * @return t_list* List of symbols, in the order where they appear in the list (unsorted by any means).
 */
t_list *
parse_elf_symbols(const t_file *file) {
    t_elf_parsing_context context = {0};
    t_elf_parse_error     ret_val = ELF_PARSE_OK;
    t_list               *symbols = NULL;

    if ((ret_val = parse_elf_hdr_ident(file, &context)) != ELF_PARSE_OK) {
        goto err;
    }
    if ((ret_val = parse_elf_hdr_shdr(file, &context)) != ELF_PARSE_OK) {
        goto err;
    }
    if ((ret_val = parse_elf_shdrs(file, &context)) != ELF_PARSE_OK) {
        goto err;
    }

    return (symbols);
err:
    ft_error(0, 0, elf_parse_error_to_string(ret_val));
    return (NULL);
}