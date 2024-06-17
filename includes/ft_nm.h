/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 10:36:54 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/17 12:24:14 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_H
#define FT_NM_H

#include <stdbool.h>
#include <stddef.h>

#include "libft.h"

#define DEFAULT_FILE_STR ("a.out")

typedef struct s_opts {
    bool debug_symbols : 1;
    bool external_symbols_only : 1;
    bool undefined_symbols_only : 1;
    bool reverse_sort : 1;
    bool no_sort : 1;
    bool display_help : 1;
} t_opts;

extern t_opts g_opts;

#endif
