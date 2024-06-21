/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbol.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:06:57 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/21 14:24:15 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "symbol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ft_nm.h"
#include "libft.h"
#include "utils.h"

static char
decode_sym_sec_type(const t_elf_parsed_shdr *sec, const char *sec_name) {
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
    /* Debug section are not specified with any sorts of flags. Also, debug section does not occupies memory during process executions. */
    if (!IS_SET(sec->flags, SHF_ALLOC) && sec_name[0] == '.') {
        if (ft_strprefix(sec_name, ".debug") || ft_strprefix(sec_name, ".zdebug") || ft_strprefix(sec_name, ".line") ||
            ft_strprefix(sec_name, ".stab") || ft_strprefix(sec_name, ".gdb_index") || ft_strprefix(sec_name, ".gnu.debuglto_.debug_") ||
            ft_strprefix(sec_name, ".gnu.linkonce.wi.")) {
            return ('N');
        }
    }
    if (sec->type != SHT_NOBITS && !IS_SET(sec->flags, SHF_WRITE)) {
        /* Symbol is in a read-only data section. */
        return ('n');
    }
    return ('?');
}

static char
decode_symbol_type(const t_sym *symbol) {
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
    if (symbol->elf_rel_shdr.type == STT_GNU_IFUNC) {
        return ('i');
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
    if (IS_SET(symbol->elf_sym.bind, STB_GNU_UNIQUE)) {
        return ('u');
    }
    if (!(IS_SET(symbol->elf_sym.bind, STB_GLOBAL) || IS_SET(symbol->elf_sym.bind, STB_LOCAL))) {
        /* If the symbol binding is not global or local, this is some processor specific semantics, since we've already checked that the
         * symbol is weak, resolve to '?'. */
        return ('?');
    }
    if (symbol->elf_sym.shndx == SHN_ABS) {
        c = 'a';
    } else {
        c = decode_sym_sec_type(&symbol->elf_rel_shdr, symbol->rel_sec_name);
    }
    if (IS_SET(symbol->elf_sym.bind, STB_GLOBAL)) {
        c = ft_toupper(c);
    }
    return (c);
}

void
print_syms(const t_syms_info *syms_info) {
    t_sym *sym  = NULL;
    char   type = 0;

    for (t_list *elem = syms_info->sym_list; elem != NULL; elem = elem->next) {
        sym  = elem->content;
        type = decode_symbol_type(sym);

        if (g_opts.undefined_symbols_only && sym->elf_sym.shndx != SHN_UNDEF) {
            continue;
        }
        if (g_opts.external_symbols_only && sym->elf_sym.bind != STB_GLOBAL) {
            continue;
        }
        if (!g_opts.debug_symbols && (sym->elf_sym.type == STT_SECTION || sym->elf_sym.type == STT_FILE || type == 'N')) {
            continue;
        }

        if (syms_info->hdr.ei_class == ELFCLASS32) {
            if (sym->elf_sym.shndx == SHN_UNDEF) {
                printf("%-8s", "");
            } else {
                printf("%.8x", (uint32_t)sym->elf_sym.value);
            }
        } else if (syms_info->hdr.ei_class == ELFCLASS64) {
            if (sym->elf_sym.shndx == SHN_UNDEF) {
                printf("%-16s", "");
            } else {
                printf("%.16lx", (uint64_t)sym->elf_sym.value);
            }
        }
        printf(" %c %s\n", type, sym->name);
    }
}

int
sort_sym(const void *a, const void *b) {
    const t_sym *sym_a = a;
    const t_sym *sym_b = b;

    return (ft_strcmp(sym_a->name, sym_b->name));
}

int
sort_sym_rev(const void *a, const void *b) {
    const t_sym *sym_a = a;
    const t_sym *sym_b = b;

    return (ft_strcmp(sym_b->name, sym_a->name));
}
