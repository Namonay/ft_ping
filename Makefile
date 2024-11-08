NAME	= ./ft_ping

SRCS	=	src/main.c \
			src/utils.c


OBJ_DIR	= objs
OBJS	= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

CC		= clang
CFLAGS	= -Wall -Wextra -Werror -I includes

DEBUG		?= false
MODE		=  "release"
TOOLCHAIN	?= clang

ifeq ($(TOOLCHAIN), gcc)
	CC = gcc
endif

ifeq ($(DEBUG), true)
	CFLAGS += -g -D DEBUG
	MODE = "debug"
endif

RM		= rm -rf

$(OBJ_DIR)/%.o: %.c
	@echo "\e[1;32m[compiling "$(MODE)" {"$(CC)"}...]\e[1;00m "$<
	@$(CC) $(CFLAGS) $(COPTS) -c $< -o $@

all:		 $(NAME)



$(NAME):	$(OBJ_DIR) $(OBJS)
	@echo "\e[1;32m[linking   "$(MODE)" {"$(CC)"}...]\e[1;00m "$@
	@$(CC) -o $(NAME) $(OBJS) -lm	
	@echo "\e[1;32m[build finished]\e[1;00m"

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

clean:
	@$(RM) $(OBJ_DIR)

fclean:		clean
	@$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re 
