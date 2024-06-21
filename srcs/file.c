/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 11:37:18 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/21 13:15:06 by plouvel          ###   ########.fr       */
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
    const char *pathname;
    uint8_t    *buf_start;
    uint8_t    *buf_end;
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
    file->buf_end  = file->buf_start + st.st_size;
    file->pathname = pathname;
    goto close_fd;
clean:
    free(file);
    file = NULL;
close_fd:
    if (fd != -1) {
        (void)close(fd);
    }
ret:
    return (file);
}

/**
 * @brief Free resources associated with a file.
 *
 * @param file The file to free.
 */
void
release_file(t_file *file) {
    if (file != NULL) {
        (void)munmap(file->buf_start, file->buf_end - file->buf_start);
        free((t_file *)file);
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
 * @note range_start must be less than range_end, and both are zero-indexed.
 */
const void *
try_read_file(const t_file *file, size_t range_start, size_t range_end) {
    assert(file != NULL);

    if (range_start >= range_end) {
        return (NULL);
    }
    if (!(file->buf_start + range_start < file->buf_end && file->buf_start + range_end <= file->buf_end)) {
        return (NULL);
    }

    return (file->buf_start + range_start);
}

/**
 * @brief Get the file size object
 *
 * @param file The file to get the size from.
 * @return size_t The size of the file.
 */
size_t
get_file_size(const t_file *file) {
    return (file->buf_end - file->buf_start);
}

/**
 * @brief Get the name of a file.
 *
 * @param file The file to get the name from.
 * @return const char* The name of the file.
 */
const char *
get_file_name(const t_file *file) {
    return (file->pathname);
}

/**
 * @brief Get the offset of a pointer in a file.
 *
 * @param file The file to get the offset from.
 * @param ptr The pointer to get the offset of.
 * @return size_t The offset of the pointer in the file.
 */
size_t
get_file_ptr_offset(const t_file *file, const void *ptr) {
    return ((const uint8_t *)ptr - file->buf_start);
}

/**
 * @brief Get a pointer from an offset in a file.
 *
 * @param file The file to get the pointer from.
 * @param offset The offset to get the pointer from.
 * @return const void* The pointer at the offset in the file.
 */
const void *
get_file_ptr_from_offset(const t_file *file, size_t offset) {
    return (file->buf_start + offset);
}

/**
 * @brief Check if a file pointer offset is valid.
 *
 * @param file The file to check the offset in.
 * @param offset The offset to check.
 * @return true if the offset is valid, false otherwise.
 */
bool
is_file_ptr_offset_valid(const t_file *file, size_t offset) {
    return (offset < get_file_size(file));
}