# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/05 11:17:26 by plouvel           #+#    #+#              #
#    Updated: 2024/06/05 11:17:28 by plouvel          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS_DIR=srcs
LIBFT_DIR=libft
OBJS_DIR=objs
INCLUDES_DIR=includes $(LIBFT_DIR)/includes

SRCS=main.c \
	 parsing/opts.c \
	 ft_nm.c

LIBFT=$(LIBFT_DIR)/libft.a

OBJS=$(addprefix $(OBJS_DIR)/, $(SRCS:.c=.o))
HEADS=$(addprefix -I, $(INCLUDES_DIR))

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
	$(CC) $(CFLAGS) $(HEADS) $(DEBUG_FLAGS) -c $< -o $@
$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR) all

.PHONY: all clean fclean re