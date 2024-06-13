/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elf_hdr.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 15:48:04 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/13 14:43:20 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse_elf.h"
#include "utils.h"

static void
parse_elf_hdr_32(const Elf32_Ehdr *hdr, t_elf_parsed_hdr *parsed_hdr) {
    uint64_t shdrs_offset     = 0;
    uint16_t shdrs_nbr        = 0;
    uint16_t shdrs_entry_size = 0;
    uint16_t shdrs_strndx     = 0;

    if (parsed_hdr->ei_endianess == ELF_ENDIAN_BIG) {
        shdrs_offset     = uint32_t_BE_to_host_byte_order(hdr->e_shoff);
        shdrs_nbr        = uint16_t_BE_to_host_byte_order(hdr->e_shnum);
        shdrs_entry_size = uint16_t_BE_to_host_byte_order(hdr->e_shentsize);
        shdrs_strndx     = uint16_t_BE_to_host_byte_order(hdr->e_shstrndx);
    } else if (parsed_hdr->ei_endianess == ELF_ENDIAN_LITTLE) {
        shdrs_offset     = uint32_t_LE_to_host_byte_order(hdr->e_shoff);
        shdrs_nbr        = uint16_t_LE_to_host_byte_order(hdr->e_shnum);
        shdrs_entry_size = uint16_t_LE_to_host_byte_order(hdr->e_shentsize);
        shdrs_strndx     = uint16_t_LE_to_host_byte_order(hdr->e_shstrndx);
    }

    parsed_hdr->shdr_tab_size     = shdrs_nbr * shdrs_entry_size;
    parsed_hdr->shdr_tab_ent_size = shdrs_entry_size;
    parsed_hdr->shdr_tab_ent_nbr  = shdrs_nbr;
    parsed_hdr->shdr_tab_off      = shdrs_offset;
    parsed_hdr->shdr_tab_strndx   = shdrs_strndx;
}

static void
parse_elf_hdr_64(const Elf64_Ehdr *header, t_elf_parsed_hdr *parsed_hdr) {
    uint64_t shdrs_offset     = 0;
    uint16_t shdrs_nbr        = 0;
    uint16_t shdrs_entry_size = 0;
    uint16_t shdrs_strndx     = 0;

    if (parsed_hdr->ei_endianess == ELF_ENDIAN_BIG) {
        shdrs_offset     = uint64_t_BE_to_host_byte_order(header->e_shoff);
        shdrs_nbr        = uint16_t_BE_to_host_byte_order(header->e_shnum);
        shdrs_entry_size = uint16_t_BE_to_host_byte_order(header->e_shentsize);
        shdrs_strndx     = uint16_t_BE_to_host_byte_order(header->e_shstrndx);
    } else if (parsed_hdr->ei_endianess == ELF_ENDIAN_LITTLE) {
        shdrs_offset     = uint64_t_LE_to_host_byte_order(header->e_shoff);
        shdrs_nbr        = uint16_t_LE_to_host_byte_order(header->e_shnum);
        shdrs_entry_size = uint16_t_LE_to_host_byte_order(header->e_shentsize);
        shdrs_strndx     = uint16_t_LE_to_host_byte_order(header->e_shstrndx);
    }

    parsed_hdr->shdr_tab_size     = shdrs_nbr * shdrs_entry_size;
    parsed_hdr->shdr_tab_ent_size = shdrs_entry_size;
    parsed_hdr->shdr_tab_ent_nbr  = shdrs_nbr;
    parsed_hdr->shdr_tab_off      = shdrs_offset;
    parsed_hdr->shdr_tab_strndx   = shdrs_strndx;
}

t_elf_parsed_hdr
parse_elf_hdr(const void *elf_hdr) {
    const uint8_t   *hdr        = (const uint8_t *)elf_hdr;
    t_elf_parsed_hdr parsed_hdr = {0};

    parsed_hdr.ei_magic[0]  = hdr[EI_MAG0];
    parsed_hdr.ei_magic[1]  = hdr[EI_MAG1];
    parsed_hdr.ei_magic[2]  = hdr[EI_MAG2];
    parsed_hdr.ei_magic[3]  = hdr[EI_MAG3];
    parsed_hdr.ei_class     = hdr[EI_CLASS];
    parsed_hdr.ei_endianess = hdr[EI_DATA];

    if (parsed_hdr.ei_class == ELFCLASS32) {
        parse_elf_hdr_32(elf_hdr, &parsed_hdr);
    } else if (parsed_hdr.ei_class == ELFCLASS64) {
        parse_elf_hdr_64(elf_hdr, &parsed_hdr);
    }

    return (parsed_hdr);
}

t_elf_parse_error
check_elf_hdr(const t_file *file, const t_elf_parsed_hdr *hdr) {
    size_t shdr_ent_min_size = 0;

    if (hdr->ei_magic[0] != ELFMAG0 || hdr->ei_magic[1] != ELFMAG1 || hdr->ei_magic[2] != ELFMAG2 || hdr->ei_magic[3] != ELFMAG3) {
        return (ELF_PARSE_INVALID_HDR_MAGIC);
    }
    if (hdr->ei_class != ELFCLASS32 && hdr->ei_class != ELFCLASS64) {
        return (ELF_PARSE_INVALID_HDR_CLASS);
    }
    if (hdr->ei_endianess != ELFDATA2LSB && hdr->ei_endianess != ELFDATA2MSB) {
        return (ELF_PARSE_INVALID_HDR_ENDIANESS);
    }
    shdr_ent_min_size = hdr->ei_class == ELFCLASS32 ? sizeof(Elf32_Shdr) : sizeof(Elf64_Shdr);
    if (hdr->shdr_tab_ent_nbr == 0) {
        return (ELF_PARSE_INVALID_HDR_SHDR_ENTRY_NBR);
    }
    if (hdr->shdr_tab_ent_size < shdr_ent_min_size) {
        return (ELF_PARSE_INVALID_HDR_SHDR_ENTRY_NBR);
    }
    if (try_read_file(file, hdr->shdr_tab_off, hdr->shdr_tab_off + hdr->shdr_tab_size) == NULL) {
        return (ELF_PARSE_INVALID_HDR_SHDRS);
    }
    if (!(hdr->shdr_tab_strndx > SHN_UNDEF && hdr->shdr_tab_strndx < hdr->shdr_tab_ent_nbr)) {
        return (ELF_PARSE_INVALID_HDR_SHDR_STRNDX);
    }

    return (ELF_PARSE_OK);
}