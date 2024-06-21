/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_opts.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 10:31:32 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/18 12:13:18 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "ft_args_parser.h"
#include "ft_nm.h"

int
parse_debug_symbols(const char *arg, t_args_parser_state *state, void *input) {
    (void)arg;
    (void)state;
    (void)input;

    g_opts.debug_symbols = true;

    return (0);
}

int
parse_external_symbols_only(const char *arg, t_args_parser_state *state, void *input) {
    (void)arg;
    (void)state;
    (void)input;

    g_opts.external_symbols_only = true;

    return (0);
}

int
parse_undefined_symbols_only(const char *arg, t_args_parser_state *state, void *input) {
    (void)arg;
    (void)state;
    (void)input;

    g_opts.undefined_symbols_only = true;

    return (0);
}

int
parse_reverse_sort(const char *arg, t_args_parser_state *state, void *input) {
    (void)arg;
    (void)state;
    (void)input;

    g_opts.reverse_sort = true;

    return (0);
}

int
parse_no_sort(const char *arg, t_args_parser_state *state, void *input) {
    (void)arg;
    (void)state;
    (void)input;

    g_opts.no_sort = true;

    return (0);
}

int
parse_help(const char *arg, t_args_parser_state *state, void *input) {
    (void)arg;
    (void)state;
    (void)input;

    g_opts.display_help = true;

    return (0);
}

int
parse_argument(const char *arg, t_args_parser_state *state, void *input) {
    (void)state;

    t_list **files = input;
    t_list  *elem  = NULL;

    elem = ft_lstnew((char *)arg);
    if (elem == NULL) {
        state->error_message = strerror(errno);
        return (1);
    }
    elem->content = (char *)arg;
    if (*files != NULL) {
        g_opts.multiple_files = true;
    }
    ft_lstadd_back(files, elem);
    return (0);
}