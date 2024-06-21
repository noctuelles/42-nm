/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 10:28:48 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/21 13:12:05 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "elf_utils.h"
#include "file.h"
#include "ft_args_parser.h"
#include "ft_nm.h"
#include "parse_elf.h"
#include "parse_opts.h"

extern char *program_invocation_short_name;

static t_args_parser_option_entry g_option_entries[] = {{
                                                            .short_key                     = "a",
                                                            .long_key                      = "debug-syms",
                                                            .has_argument                  = false,
                                                            .description                   = "Display debugger-only symbols",
                                                            .long_key_argument_description = NULL,
                                                            .parse_fn                      = parse_debug_symbols,
                                                        },
                                                        {
                                                            .short_key                     = "g",
                                                            .long_key                      = "extern-only",
                                                            .has_argument                  = false,
                                                            .description                   = "Display only external symbols",
                                                            .long_key_argument_description = NULL,
                                                            .parse_fn                      = parse_external_symbols_only,
                                                        },
                                                        {
                                                            .short_key                     = "u",
                                                            .long_key                      = "undefined-only",
                                                            .has_argument                  = false,
                                                            .description                   = "Display only undefined symbols",
                                                            .long_key_argument_description = NULL,
                                                            .parse_fn                      = parse_undefined_symbols_only,
                                                        },
                                                        {
                                                            .short_key                     = "r",
                                                            .long_key                      = "reverse-sort",
                                                            .has_argument                  = false,
                                                            .description                   = "Reverse the sense of the sort",
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
                                                            .short_key                     = "h",
                                                            .long_key                      = "help",
                                                            .has_argument                  = false,
                                                            .description                   = "Display this help message",
                                                            .long_key_argument_description = NULL,
                                                            .parse_fn                      = parse_help,
                                                        }};

t_opts g_opts = {
    .debug_symbols          = false,
    .external_symbols_only  = false,
    .undefined_symbols_only = false,
    .reverse_sort           = false,
    .no_sort                = false,
    .display_help           = false,
};

static int
display_help(const t_args_parser_config *config) {
    printf("Usage: %s [option(s)] [file(s)]\n", program_invocation_short_name);
    printf(" List symbols in [file(s)] (%s by default)\n", DEFAULT_FILE_STR);
    printf(" The options are:\n");
    ft_args_parser_print_docs(config);
    return (0);
}

static int
display_files_symbol(t_list *files) {
    t_file *curr_file = NULL;
    size_t  nbr_files = 0;
    int     ret_val   = 0;

    nbr_files = ft_lstsize(files);
    for (t_list *elem = files; elem != NULL; elem = elem->next) {
        curr_file = load_file(elem->content);
        if (curr_file == NULL) {
            continue;
        }
        if (nbr_files > 1) {
            printf("\n%s:\n", get_file_name(curr_file));
        }
        if (dump_elf_syms(curr_file) != 0) {
            ret_val = 1;
        }
        release_file(curr_file);
    }
    return (ret_val);
}

int
main(int argc, char **argv) {
    t_list              *files   = NULL;
    t_args_parser_config config  = {.argv                      = argv,
                                    .argc                      = argc,
                                    .entries                   = g_option_entries,
                                    .entries_nbr               = sizeof(g_option_entries) / sizeof(g_option_entries[0]),
                                    .default_argument_parse_fn = parse_argument,
                                    .input                     = &files};
    int                  ret_val = 0;

    if (ft_args_parser(&config) != 0) {
        return (1);
    }
    if (g_opts.display_help) {
        return (display_help(&config));
    }
    if (files == NULL) {
        ft_lstadd_back(&files, ft_lstnew(DEFAULT_FILE_STR));
    }
    ret_val = display_files_symbol(files);
    ft_lstclear(&files, NULL);
    return (ret_val);
}
