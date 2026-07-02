NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC_DIR = coders
SRCS = $(SRC_DIR)/main.c \
	$(SRC_DIR)/parse.c \
	$(SRC_DIR)/time.c \
	$(SRC_DIR)/log.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(SRC_DIR)/codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) $(NAME).exe

re: fclean all

.PHONY: all clean fclean re
