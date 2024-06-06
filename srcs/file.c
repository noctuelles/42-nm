/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 11:37:18 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/06 15:33:27 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libft.h"

typedef struct s_file {
    uint8_t *buf_start;
    uint8_t *buf_end;
} t_file;

/**
 * @brief Load a file into memory.
 *
 * @param pathname The path to the file to load.
 * @return t_file* A pointer to the file if it was successfully loaded, or NULL if an error occured.
 */
t_file *
load_file(const char *pathname) {
    assert(pathname != NULL);

    t_file     *file = NULL;
    struct stat st   = {0};
    int         fd   = -1;

    if ((file = malloc(sizeof(t_file))) == NULL) {
        ft_error(0, errno, "malloc");
        goto ret;
    }
    if ((fd = open(pathname, O_RDONLY)) == -1) {
        ft_error(0, errno, "cannot open '%s'", pathname);
        goto clean;
    }
    if (fstat(fd, &st) == -1) {
        ft_error(0, errno, "fstat");
        goto clean;
    }
    if (S_ISDIR(st.st_mode)) {
        ft_error(0, 0, "Warning: '%s' is a directory", pathname);
        goto clean;
    }
    if (!S_ISREG(st.st_mode) == 0 && !S_ISLNK(st.st_mode) == 0) {
        ft_error(0, 0, "Warning: '%s' is not an ordinary file", pathname);
        goto clean;
    }
    if (st.st_size < 0) {
        ft_error(0, 0, "Warning: '%s' has a negative size, probably it is too large", pathname);
        goto clean;
    }
    if ((file->buf_start = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
        ft_error(0, errno, "mmap");
        goto clean;
    }
    file->buf_end = file->buf_start + st.st_size;
    goto close_fd;
clean:
    free(file);
    file = NULL;
close_fd:
    (void)close(fd);
ret:
    return (file);
}

/**
 * @brief Free resources associated with a file.
 *
 * @param file The file to free.
 */
void
free_file(t_file *file) {
    if (file != NULL) {
        (void)munmap(file->buf_start, file->buf_end - file->buf_start);
        free(file);
    }
}

/**
 * @brief Try to read a file from a range of bytes.
 *
 * @param file The file to read.
 * @param range_start The start of the range, inclusive.
 * @param range_end The end of the range, exclusive.
 * @return const void* A pointer to the start of the range if it is valid, or
 * NULL otherwise.
 *
 * @note range_start must be less than range_end.
 */
const void *
try_read_file(const t_file *file, size_t range_start, size_t range_end) {
    assert(file != NULL);
    assert(range_start < range_end);

    if (range_start + range_end > (size_t)(file->buf_end - file->buf_start)) {
        return (NULL);
    }
    return (file->buf_start + range_start);
}

size_t
get_file_size(const t_file *file) {
    return (file->buf_end - file->buf_start);
}
