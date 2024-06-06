/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 16:15:50 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/06 16:27:20 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_H
#define CONF_H

/* LITTLE_ENDIAN and BIG_ENDIAN match the output of the lscpu command. */

#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN 3412

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#endif