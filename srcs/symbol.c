/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbol.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:06:57 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/14 13:10:43 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "symbol.h"

#include <stdio.h>
#include <stdlib.h>

#include "libft.h"
#include "utils.h"

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

static char
decode_sym_sec_type(const t_elf_parsed_shdr *sec) {
    if (IS_SET(sec->flags, SHF_EXECINSTR)) {
        /* Symbol is in the text(code) section. */
        return ('t');
    }
    if (sec->type != SHT_NOBITS && IS_SET(sec->flags, SHF_ALLOC)) {
        if (!IS_SET(sec->flags, SHF_WRITE)) {
            /* Symbol is in the read-only data section. */
            return ('r');
        } else {
            /* Symbol is in the initialized data section. */
            return ('d');
        }
    }
    if (sec->type == SHT_NOBITS) {
        /* Symbol is in the BSS data section. */
        return ('b');
    }
    /* TODO: Debugs symbols... */
    if (sec->type != SHT_NOBITS && !IS_SET(sec->flags, SHF_WRITE)) {
        /* Symbol is in a read-only data section. */
        return ('n');
    }
    return ('?');
}

static char
decode_sym_type(const t_sym *symbol) {
    char c = '\0';

    if (symbol->elf_sym.shndx == SHN_COMMON) {
        /* Small Common Section doesn't seems to concern x86_64 ELF Linux. */
        return ('C');
    }
    if (symbol->elf_sym.shndx == SHN_UNDEF) {
        if (IS_SET(symbol->elf_sym.bind, STB_WEAK)) {
            if (IS_SET(symbol->elf_sym.type, STT_OBJECT)) {
                /* The symbol is a weak object, with no default value specified. */
                return ('v');
            } else {
                /* The symbol is a weak symbol, with no default value specified. */
                return ('w');
            }
        } else {
            /* The symbol is an undefined symbol. */
            return ('U');
        }
    }
    if (IS_SET(symbol->elf_sym.bind, STB_WEAK)) {
        if (IS_SET(symbol->elf_sym.type, STT_OBJECT)) {
            /* The symbol is a weak object, with a default value specified. */
            return ('V');
        } else {
            /* The symbol is a weak symbol, with a default value specified. */
            return ('W');
        }
    }
    if (!(IS_SET(symbol->elf_sym.bind, STB_GLOBAL) || IS_SET(symbol->elf_sym.bind, STB_LOCAL))) {
        /* If the symbol binding is not global or local, this is some processor specific semantics, since we've already checked that the
         * symbol is weak, resolve to '?'. */
        return ('?');
    }
    if (symbol->elf_sym.shndx == SHN_ABS) {
        /* The symbol is absolute and should not be relocated. */
        return ('a');
    }
    /* At this point, the symbol is related to a section that exists in the section header table. */
    c = decode_sym_sec_type(&symbol->elf_rel_shdr);
    if (IS_SET(symbol->elf_sym.bind, STB_GLOBAL)) {
        c = ft_toupper(c);
    }
    return (c);
}

void
print_syms(const t_syms_info *syms_info) {
    t_list *elem = NULL;
    t_sym  *sym  = NULL;
    char    type = '\0';

    elem = syms_info->sym_list;
    while (elem != NULL) {
        sym  = elem->content;
        type = decode_sym_type(sym);
        if (sym->elf_sym.shndx == SHN_UNDEF) {
            printf("                 %c %s\n", type, sym->name);
        } else {
            printf("%016lx %c %s\n", sym->elf_sym.value, type, sym->name);
        }
        elem = elem->next;
    }
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
        elem = elem->next;
    }
    return (ret_val);
}
