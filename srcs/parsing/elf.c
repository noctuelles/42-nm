/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:05:22 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/06 15:59:49 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/elf.h"

#include "file.h"
#include "libft.h"

t_elf_parsing_context
parse_elf_hdr_ident(const t_file *file) {
    Elf32_Ehdr           *header  = NULL;
    t_elf_parsing_context context = {0};

    const uint8_t magic[]            = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
    const uint8_t possible_classes[] = {ELFCLASS32, ELFCLASS64};
    const uint8_t possible_endians[] = {ELFDATA2LSB, ELFDATA2MSB};

    header = try_read_file(file, 0, sizeof(*header));
    if (header == NULL) {
        // File too short
        return;
    }

    // TODO: check the corecteness of the class and endianess
    context.class     = header->e_ident[EI_CLASS];
    context.endianess = header->e_ident[EI_DATA];

    return (context);
}

void
parse_elf_hdr_section_hdrs(const t_file *file, const t_elf_parsing_context *parsing_context) {
    Elf64_Ehdr *hdr         = NULL;
    Elf64_Shdr *section_hdr = NULL;
    Elf64_Off   offset      = 0;
    size_t      sh_size     = 0;

    hdr = try_read_file(file, 0, sizeof(*hdr));
    if (hdr == NULL) {
        // File too short
        return;
    }
    // TODO: endianess !
    sh_size     = hdr->e_shnum * hdr->e_shentsize;
    section_hdr = try_read_file(file, (size_t)hdr->e_shoff, sh_size);
    if (section_hdr == NULL) {
        // File too short
        return;
    }
}
