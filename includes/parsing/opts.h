/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opts.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 10:39:19 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/05 12:12:44 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OPTS_H
#define OPTS_H

#include "ft_args_parser.h"

int parse_debug_symbols(const char *arg, t_args_parser_state *state, void *input);
int parse_external_symbols_only(const char *arg, t_args_parser_state *state, void *input);
int parse_undefined_symbols_only(const char *arg, t_args_parser_state *state, void *input);
int parse_reverse_sort(const char *arg, t_args_parser_state *state, void *input);
int parse_no_sort(const char *arg, t_args_parser_state *state, void *input);
int parse_help(const char *arg, t_args_parser_state *state, void *input);
int parse_argument(const char *arg, t_args_parser_state *state, void *input);

#endif
