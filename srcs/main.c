/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 10:28:48 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/05 11:17:01 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_args_parser.h"
#include "ft_nm.h"
#include "parsing/opts.h"

static t_args_parser_option_entry g_option_entries[] = {
    {
        .short_key = "a",
        .long_key = "debug-syms",
        .has_argument = false,
        .description = "Display debugger-only symbols",
        .long_key_argument_description = NULL,
        .parse_fn = parse_debug_symbols,
    },
    {
        .short_key = "g",
        .long_key = "extern-only",
        .has_argument = false,
        .description = "Display only external symbols",
        .long_key_argument_description = NULL,
        .parse_fn = parse_external_symbols_only,
    },
    {
        .short_key = "u",
        .long_key = "undefined-only",
        .has_argument = false,
        .description = "Display only undefined symbols",
        .long_key_argument_description = NULL,
        .parse_fn = parse_undefined_symbols_only,
    },
    {
        .short_key = "r",
        .long_key = "reverse-sort",
        .has_argument = false,
        .description = "Reverse the sense of the sort",
        .long_key_argument_description = NULL,
        .parse_fn = parse_reverse_sort,
    },
    {
        .short_key = "p",
        .long_key = "no-sort",
        .has_argument = false,
        .description = "Do not sort the symbols",
        .long_key_argument_description = NULL,
        .parse_fn = parse_no_sort,
    }};

int main(int argc, char **argv) {
  t_ft_nm ft_nm = {0};
  t_args_parser_config config = {.argv = argv,
                                 .argc = argc,
                                 .entries = g_option_entries,
                                 .entries_nbr = sizeof(g_option_entries) /
                                                sizeof(g_option_entries[0]),
                                 .default_argument_parse_fn = parse_argument,
                                 .input = &ft_nm};
  int ret_val = -1;

  if (ft_args_parser(&config) != 0) {
    goto ret;
  }

  for (t_list *elem = ft_nm.files; elem != NULL; elem = elem->next) {
    // Extract the sections from the file...
  }

  ret_val = 0;
ret:
  free_ft_nm(&ft_nm);
  return (ret_val);
}
