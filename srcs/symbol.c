/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbol.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:06:57 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/13 12:41:24 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "symbol.h"

#include <stdlib.h>

#include "libft.h"

t_list *
new_symbol_elem() {
    t_list *elem   = NULL;
    t_sym  *symbol = NULL;

    if ((symbol = malloc(sizeof(*symbol))) == NULL) {
        return (NULL);
    }
    if ((elem = ft_lstnew(symbol)) == NULL) {
        free(symbol);
        return (NULL);
    }
    return (elem);
}
