/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 10:28:48 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/06 21:56:54 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "elf_utils.h"
#include "file.h"
#include "ft_args_parser.h"
#include "ft_nm.h"
#include "parsing/opts.h"

extern char *program_invocation_short_name;

static t_args_parser_option_entry g_option_entries[] = {{
                                                            .short_key    = "a",
                                                            .long_key     = "debug-syms",
                                                            .has_argument = false,
                                                            .description  = "Display debugger-only symbols",
                                                            .long_key_argument_description = NULL,
                                                            .parse_fn                      = parse_debug_symbols,
                                                        },
                                                        {
                                                            .short_key    = "g",
                                                            .long_key     = "extern-only",
                                                            .has_argument = false,
                                                            .description  = "Display only external symbols",
                                                            .long_key_argument_description = NULL,
                                                            .parse_fn = parse_external_symbols_only,
                                                        },
                                                        {
                                                            .short_key    = "u",
                                                            .long_key     = "undefined-only",
                                                            .has_argument = false,
                                                            .description  = "Display only undefined symbols",
                                                            .long_key_argument_description = NULL,
                                                            .parse_fn = parse_undefined_symbols_only,
                                                        },
                                                        {
                                                            .short_key    = "r",
                                                            .long_key     = "reverse-sort",
                                                            .has_argument = false,
                                                            .description  = "Reverse the sense of the sort",
                                                            .long_key_argument_description = NULL,
                                                            .parse_fn                      = parse_reverse_sort,
                                                        },
                                                        {
                                                            .short_key                     = "p",
                                                            .long_key                      = "no-sort",
                                                            .has_argument                  = false,
                                                            .description                   = "Do not sort the symbols",
                                                            .long_key_argument_description = NULL,
                                                            .parse_fn                      = parse_no_sort,
                                                        },
                                                        {
                                                            .short_key    = "h",
                                                            .long_key     = "help",
                                                            .has_argument = false,
                                                            .description  = "Display this help message",
                                                            .long_key_argument_description = NULL,
                                                            .parse_fn                      = parse_help,
                                                        }};

static int
display_help(const t_args_parser_config *config) {
    printf("Usage: %s [option(s)] [file(s)]\n", program_invocation_short_name);
    printf(" List symbols in [file(s)] (%s by default)\n", DEFAULT_FILE_STR);
    printf(" The options are:\n");
    ft_args_parser_print_docs(config);
    return (0);
}

int
main(int argc, char **argv) {
    t_ft_nm              ft_nm     = {0};
    t_args_parser_config config    = {.argv                      = argv,
                                      .argc                      = argc,
                                      .entries                   = g_option_entries,
                                      .entries_nbr               = sizeof(g_option_entries) / sizeof(g_option_entries[0]),
                                      .default_argument_parse_fn = parse_argument,
                                      .input                     = &ft_nm};
    int                  ret_val   = -1;
    const t_file        *curr_file = NULL;
    const char          *pathname  = NULL;

    if (ft_args_parser(&config) != 0) {
        goto ret;
    }
    if (ft_nm.opts.display_help) {
        ret_val = display_help(&config);
        goto ret;
    }
    if (ft_nm.files == NULL) {
        ft_lstadd_back(&ft_nm.files, ft_lstnew(DEFAULT_FILE_STR));
    }
    for (t_list *elem = ft_nm.files; elem != NULL; elem = elem->next) {
        pathname  = elem->content;
        curr_file = load_file(pathname);
        if (curr_file == NULL) {
            continue;
        }
        if (ft_lstsize(ft_nm.files) > 1) {
            printf("\n%s:\n", pathname);
        }

        const Elf64_Ehdr *header = try_read_file(curr_file, 0, sizeof(*header));
        if (header == NULL) {
            return (1);
        }
        const Elf64_Shdr *section_hdr =
            try_read_file(curr_file, (size_t)header->e_shoff, header->e_shoff + header->e_shnum * header->e_shentsize);
        if (section_hdr == NULL) {
            return (1);
        }
        for (size_t i = 0; i < header->e_shnum; i++) {
            if (section_hdr[i].sh_type != SHT_SYMTAB) {
                continue;
            }
            char *chr = (char *)header + section_hdr[header->e_shstrndx].sh_offset + section_hdr[i].sh_name;

            printf("Section %s\n", chr);
        }

        // elf parsing...

        free_file(curr_file);
    }
    ret_val = 0;
ret:
    free_ft_nm(&ft_nm);
    return (ret_val);
}
