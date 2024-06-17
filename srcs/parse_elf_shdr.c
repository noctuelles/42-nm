/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elf_shdr.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 15:43:54 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/17 14:54:12 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse_elf.h"
#include "utils.h"

static void
parse_elf_shdr_32(const Elf32_Shdr *shdr, const t_elf_parsed_hdr *parsed_hdr, t_elf_parsed_shdr *parsed_shdr) {
    Elf32_Off off      = shdr->sh_offset;
    uint32_t  ent_size = shdr->sh_entsize;
    uint32_t  size     = shdr->sh_size;
    uint32_t  link_val = shdr->sh_link;
    uint32_t  flags    = shdr->sh_flags;
    uint32_t  type     = shdr->sh_type;
    uint32_t  name     = shdr->sh_name;

    if (parsed_hdr->ei_endianess == ELFDATA2MSB) {
        off      = uint32_t_BE_to_host_byte_order(off);
        ent_size = uint32_t_BE_to_host_byte_order(ent_size);
        size     = uint32_t_BE_to_host_byte_order(size);
        link_val = uint32_t_BE_to_host_byte_order(link_val);
        flags    = uint32_t_BE_to_host_byte_order(flags);
        type     = uint32_t_BE_to_host_byte_order(type);
        name     = uint32_t_BE_to_host_byte_order(name);
    } else if (parsed_hdr->ei_endianess == ELFDATA2LSB) {
        off      = uint32_t_LE_to_host_byte_order(off);
        ent_size = uint32_t_LE_to_host_byte_order(ent_size);
        size     = uint32_t_LE_to_host_byte_order(size);
        link_val = uint32_t_LE_to_host_byte_order(link_val);
        flags    = uint32_t_LE_to_host_byte_order(flags);
        type     = uint32_t_LE_to_host_byte_order(type);
        name     = uint32_t_LE_to_host_byte_order(name);
    }

    parsed_shdr->ent_size = (size_t)ent_size;
    parsed_shdr->size     = (size_t)size;
    parsed_shdr->link_val = (size_t)link_val;
    parsed_shdr->flags    = (size_t)flags;
    parsed_shdr->type     = (size_t)type;
    parsed_shdr->offset   = (size_t)off;
    parsed_shdr->name     = name;
}

static void
parse_elf_shdr_64(const Elf64_Shdr *shdr, const t_elf_parsed_hdr *parsed_hdr, t_elf_parsed_shdr *parsed_shdr) {
    Elf64_Off off      = shdr->sh_offset;
    uint64_t  ent_size = shdr->sh_entsize;
    uint64_t  size     = shdr->sh_size;
    uint64_t  link_val = shdr->sh_link;
    uint64_t  flags    = shdr->sh_flags;
    uint32_t  type     = shdr->sh_type;
    uint32_t  name     = shdr->sh_name;

    if (parsed_hdr->ei_endianess == ELFDATA2MSB) {
        off      = uint64_t_BE_to_host_byte_order(off);
        ent_size = uint64_t_BE_to_host_byte_order(ent_size);
        size     = uint64_t_BE_to_host_byte_order(size);
        link_val = uint64_t_BE_to_host_byte_order(link_val);
        flags    = uint64_t_BE_to_host_byte_order(flags);
        type     = uint32_t_BE_to_host_byte_order(type);
        name     = uint32_t_BE_to_host_byte_order(name);
    } else if (parsed_hdr->ei_endianess == ELFDATA2LSB) {
        off      = uint64_t_LE_to_host_byte_order(off);
        ent_size = uint64_t_LE_to_host_byte_order(ent_size);
        size     = uint64_t_LE_to_host_byte_order(size);
        link_val = uint64_t_LE_to_host_byte_order(link_val);
        flags    = uint64_t_LE_to_host_byte_order(flags);
        type     = uint32_t_LE_to_host_byte_order(type);
        name     = uint32_t_LE_to_host_byte_order(name);
    }

    parsed_shdr->ent_size = ent_size;
    parsed_shdr->size     = size;
    parsed_shdr->link_val = link_val;
    parsed_shdr->flags    = flags;
    parsed_shdr->type     = type;
    parsed_shdr->offset   = off;
    parsed_shdr->name     = name;
}

/**
 * @brief Dispatcher function for parsing a section header.
 *
 * @param shdr Section header pointer.
 * @param context Context of the parsing.
 * @param parsed_shdr Result-argument storing the parsing result.
 */
t_elf_parsed_shdr
parse_elf_shdr(const void *shdr, const t_elf_parsed_hdr *hdr) {
    t_elf_parsed_shdr parsed_shdr = {0};

    if (hdr->ei_class == ELFCLASS32) {
        parse_elf_shdr_32(shdr, hdr, &parsed_shdr);
    } else if (hdr->ei_class == ELFCLASS64) {
        parse_elf_shdr_64(shdr, hdr, &parsed_shdr);
    }

    return (parsed_shdr);
}

t_elf_parse_error
check_elf_shdr_symtab(const t_file *file, const t_elf_parsed_shdr *shdr, const t_elf_parsed_hdr *hdr) {
    size_t min_symtab_ent_size = 0;

    if (hdr->ei_class == ELFCLASS32) {
        min_symtab_ent_size = sizeof(Elf32_Sym);
    } else if (hdr->ei_class == ELFCLASS64) {
        min_symtab_ent_size = sizeof(Elf64_Sym);
    }
    if (shdr->ent_size < min_symtab_ent_size) {
        return (ELF_PARSE_UNRECOGNIZED_FORMAT);
    }
    if (shdr->size % shdr->ent_size != 0) {
        return (ELF_PARSE_UNRECOGNIZED_FORMAT);
    }
    if (!(shdr->link_val > 0 && shdr->link_val < hdr->shdr_tab_ent_nbr)) {
        return (ELF_PARSE_UNRECOGNIZED_FORMAT);
    }
    if (try_read_file(file, shdr->offset, shdr->offset + shdr->size) == NULL) {
        return (ELF_PARSE_FILE_TOO_SHORT);
    }
    return (ELF_PARSE_OK);
}

t_elf_parse_error
check_elf_shdr_strtab(const t_file *file, const t_elf_parsed_shdr *shdr) {
    if (shdr->type != SHT_STRTAB) {
        return (ELF_PARSE_UNRECOGNIZED_FORMAT);
    }
    if (try_read_file(file, shdr->offset, shdr->offset + shdr->size) == NULL) {
        return (ELF_PARSE_FILE_TOO_SHORT);
    }
    return (ELF_PARSE_OK);
}