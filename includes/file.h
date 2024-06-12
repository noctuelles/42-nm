/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 11:55:33 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/12 12:27:42 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_H
#define FILE_H

/**
 * @file file.h  Abstraction for file handling.
 */

#include <stddef.h>
#include <stdint.h>

typedef struct s_file t_file;

t_file *load_file(const char *pathname);
void    free_file(const t_file *file);

const void *try_read_file(const t_file *file, size_t range_start, size_t range_end);
size_t      get_file_size(const t_file *file);
size_t      get_file_ptr_offset(const t_file *file, const void *ptr);

const void *get_file_ptr_from_offset(const t_file *file, size_t offset);

#endif