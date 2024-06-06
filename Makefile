# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/05 11:17:26 by plouvel           #+#    #+#              #
#    Updated: 2024/06/06 16:25:29 by plouvel          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#Paths

BUILD_DIR=build
OBJS_DIR=$(BUILD_DIR)/objs
DEPS_DIR=$(BUILD_DIR)/deps

SRCS_DIR=srcs
LIBFT_DIR=libft
INCLUDES_DIR=includes $(LIBFT_DIR)/includes

SRCS=main.c \
	 parsing/opts.c \
	 ft_nm.c \
	 file.c
LIBFT_SRCS = $(wildcard $(LIBFT_DIR)/srcs/**/*.c)
LIBFT=$(LIBFT_DIR)/libft.a

OBJS=$(addprefix $(OBJS_DIR)/, $(SRCS:.c=.o))
DEPS=$(OBJS:.o=.d)
HEADS=$(addprefix -I, $(INCLUDES_DIR))


# Byte Order

BYTE_ORDER=$(shell lscpu | grep "Byte Order:" | awk '{print $$3 $$4}')

ifeq ($(BYTE_ORDER),LittleEndian)
	BYTE_ORDER=1234
else ifeq ($(BYTE_ORDER),BigEndian)
	BYTE_ORDER=3412
else
	$(error Unrecognized byte order)
endif

NAME=ft_nm

CC=gcc
RM=rm -rf
MKDIR=mkdir -p

CFLAGS=-Wall -Werror -Wextra -Wpedantic

DEBUG_FLAGS=-g3

all: $(NAME)
clean:
	$(RM) $(OBJS_DIR)
fclean: clean
	$(RM) $(NAME)
re: fclean all

$(NAME): $(LIBFT) $(OBJS) Makefile
	$(CC) $(OBJS) $(LIBFT) $(DEBUG_FLAGS) -L $(LIBFT_DIR) -lft -o $@
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(HEADS) -MMD $(DEBUG_FLAGS) -DBYTE_ORDER=$(BYTE_ORDER) -c $< -o $@
$(LIBFT): $(LIBFT_SRCS)
	$(MAKE) -C $(LIBFT_DIR) all

-include $(DEPS)

.PHONY: all clean fclean re