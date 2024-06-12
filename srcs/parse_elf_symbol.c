/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elf_symbol.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 15:41:50 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/12 14:18:39 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse_elf.h"
#include "utils.h"

static void
parse_elf_symbol_32(const Elf64_Sym *sym, const t_elf_parsed_hdr *parsed_hdr, t_elf_parsed_symbol *parsed_symbol) {
    Elf32_Addr value = sym->st_value;
    uint32_t   size  = sym->st_size;
    uint32_t   name  = sym->st_name;
    uint16_t   shndx = sym->st_shndx;
    uint8_t    info  = sym->st_info;
    uint8_t    other = sym->st_other;

    if (parsed_hdr->ei_endianess == ELFDATA2MSB) {
        value = uint32_t_BE_to_host_byte_order(value);
        size  = uint32_t_BE_to_host_byte_order(size);
        name  = uint32_t_BE_to_host_byte_order(name);
        shndx = uint16_t_BE_to_host_byte_order(shndx);
    } else if (parsed_hdr->ei_endianess == ELFDATA2LSB) {
        value = uint32_t_LE_to_host_byte_order(value);
        size  = uint32_t_LE_to_host_byte_order(size);
        name  = uint32_t_LE_to_host_byte_order(name);
        shndx = uint16_t_LE_to_host_byte_order(shndx);
    }

    parsed_symbol->value = (size_t)value;
    parsed_symbol->size  = (size_t)size;
    parsed_symbol->name  = name;
    parsed_symbol->shndx = shndx;
    parsed_symbol->info  = info;
    parsed_symbol->other = other;
}

static void
parse_elf_symbol_64(const Elf64_Sym *sym, const t_elf_parsed_hdr *parsed_hdr, t_elf_parsed_symbol *parsed_symbol) {
    Elf64_Addr value = sym->st_value;
    uint64_t   size  = sym->st_size;
    uint32_t   name  = sym->st_name;
    uint16_t   shndx = sym->st_shndx;
    uint8_t    info  = sym->st_info;
    uint8_t    other = sym->st_other;

    if (parsed_hdr->ei_endianess == ELFDATA2MSB) {
        value = uint64_t_BE_to_host_byte_order(value);
        size  = uint64_t_BE_to_host_byte_order(size);
        name  = uint32_t_BE_to_host_byte_order(name);
        shndx = uint16_t_BE_to_host_byte_order(shndx);
    } else if (parsed_hdr->ei_endianess == ELFDATA2LSB) {
        value = uint64_t_LE_to_host_byte_order(value);
        size  = uint64_t_LE_to_host_byte_order(size);
        name  = uint32_t_LE_to_host_byte_order(name);
        shndx = uint16_t_LE_to_host_byte_order(shndx);
    }

    parsed_symbol->value = (size_t)value;
    parsed_symbol->size  = (size_t)size;
    parsed_symbol->name  = name;
    parsed_symbol->shndx = shndx;
    parsed_symbol->info  = info;
    parsed_symbol->other = other;
}

t_elf_parsed_symbol
parse_elf_symbol(const void *symbol, const t_elf_parsed_hdr *parsed_hdr) {
    t_elf_parsed_symbol parsed_symbol = {0};

    if (parsed_hdr->ei_class == ELFCLASS32) {
        parse_elf_symbol_32(symbol, parsed_hdr, &parsed_symbol);
    } else if (parsed_hdr->ei_class == ELFCLASS64) {
        parse_elf_symbol_64(symbol, parsed_hdr, &parsed_symbol);
    }

    return (parsed_symbol);
}