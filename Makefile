# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/05 11:17:26 by plouvel           #+#    #+#              #
#    Updated: 2024/06/07 14:26:58 by plouvel          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Paths

BUILD_PATH=build
TESTS_PATH=tests
INCLUDES_PATH=includes
SRCS_PATH=srcs
LIBFT_PATH=libft

DEPS_PATH=$(BUILD_PATH)/deps
OBJS_PATH=$(BUILD_PATH)/objs
TEST_RESULTS_PATH=$(BUILD_PATH)/test_results

UNITY_PATH=unity/src

PROJECT_INCLUDE=$(addprefix -I, $(INCLUDES_PATH) $(LIBFT_PATH)/includes)
UNITY_INCLUDE=$(addprefix -I, $(UNITY_PATH))
ALL_INCLUDE=$(PROJECT_INCLUDE) $(UNITY_INCLUDE)

# Sources

SRCS=main.c \
	 parsing/opts.c \
	 parsing/elf.c \
	 ft_nm.c \
	 utils.c \
	 file.c
SRCS_TEST=$(addprefix $(TESTS_PATH)/, parsing/test_elf.c test_utils.c test_file.c)
LIBFT_SRCS = $(wildcard $(LIBFT_PATH)/srcs/**/*.c)

# Output

OBJS=$(addprefix $(OBJS_PATH)/, $(SRCS:.c=.o))
TEST_RESULTS=$(patsubst $(TESTS_PATH)/test_%.c, $(TEST_RESULTS_PATH)/test_%.txt,$(SRCS_TEST))
HEADS=$(addprefix -I, $(INCLUDES_DIR))

$(info $(TEST_RESULTS))

# Byte Order

BYTE_ORDER=$(shell lscpu | grep "Byte Order:" | awk '{print $$3 $$4}')

ifeq ($(BYTE_ORDER),LittleEndian)
	BYTE_ORDER=1234
else ifeq ($(BYTE_ORDER),BigEndian)
	BYTE_ORDER=3412
else
	$(error Unrecognized byte order)
endif

# Compilation

NAME=ft_nm

CC=gcc
LINK=gcc
DEPENDS=gcc -MM -MG -MF

CFLAGS=-Wall -Werror -Wextra -Wpedantic

DEBUG_FLAGS=-g3

# Misc

PASSED = `grep -s PASS $(TEST_RESULTS_PATH)/*.txt`
FAIL = `grep -s FAIL $(TEST_RESULTS_PATH)/*.txt`
IGNORE = `grep -s IGNORE $(TEST_RESULTS_PATH)/*.txt`

MKDIR=mkdir -p
RM=rm -rf

TARGET_EXTENSION=out

# Libs

LIBFT=$(LIBFT_PATH)/libft.a

# Rules

all: $(NAME)
clean:
	$(RM) $(BUILD_PATH)
fclean: clean
	$(RM) $(NAME)
re: fclean all

test: $(TEST_RESULTS_PATH) $(TEST_RESULTS)
	@echo "-----------------------\n\tIGNORED ⭕\n-----------------------"
	@echo "$(IGNORE)"
	@echo "-----------------------\n\tFAILURES ❌\n-----------------------"
	@echo "$(FAIL)"
	@echo "-----------------------\n\tPASSED ✅\n-----------------------"
	@echo "$(PASSED)"

#Rule to link the project
$(NAME): $(OBJS) $(LIBFT)
	$(LINK) -o $@ $^

# Rule to generate the test results
$(TEST_RESULTS_PATH)/%.txt: $(BUILD_PATH)/%.$(TARGET_EXTENSION)
	-./$< > $@ 2>&1

# Rule to link the test runner
$(BUILD_PATH)/test_%.$(TARGET_EXTENSION): $(OBJS_PATH)/test_%.o $(OBJS_PATH)/unity.o $(OBJS_PATH)/%.o $(LIBFT)
	$(LINK) -o $@ $^ 

# Rule to compile the files of the project
$(OBJS_PATH)/%.o:: $(SRCS_PATH)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(PROJECT_INCLUDE) -DBYTE_ORDER=$(BYTE_ORDER) -c $< -o $@

# Rule to compile the Unity Framework
$(OBJS_PATH)/%.o:: $(UNITY_PATH)/%.c $(UNITY_PATH)/%.h | $(OBJS_PATH)
	$(CC) $(UNITY_INCLUDE) -c $< -o $@

# Rule to compile the tests of the project
$(OBJS_PATH)/%.o:: $(TESTS_PATH)/%.c | $(OBJS_PATH)
	$(CC) $(DEBUG_FLAGS) $(ALL_INCLUDE) -DBYTE_ORDER=$(BYTE_ORDER) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_PATH)

# Rules to create directories

$(OBJS_PATH):
	$(MKDIR) $@

$(TEST_RESULTS_PATH):
	$(MKDIR) $@

$(DEPS_PATH):
	$(MKDIR) $@

.PHONY: all clean fclean re
.PRECIOUS: $(BUILD_PATH)/test_%.$(TARGET_EXTENSION)
.PRECIOUS: $(OBJS_PATH)/%.o
.PRECIOUS: $(DEPS_PATH)/%.d
.PRECIOUS: $(TEST_RESULTS_PATH)/%.txt