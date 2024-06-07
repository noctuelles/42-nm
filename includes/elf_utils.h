/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_utils.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:45:28 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/07 14:58:52 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ELF_UTILS_H
#define ELF_UTILS_H

#include <elf.h>

typedef enum e_elf_class {
    ELF_CLASS_UNDEF = ELFCLASSNONE,
    ELF_CLASS_32    = ELFCLASS32,
    ELF_CLASS_64    = ELFCLASS64,
} t_elf_class;

typedef enum e_elf_endianess {
    ELF_ENDIAN_UNDEF  = ELFDATANONE,
    ELF_ENDIAN_LITTLE = ELFDATA2LSB,
    ELF_ENDIAN_BIG    = ELFDATA2MSB,
} t_elf_endianess;

#endif