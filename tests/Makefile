NAME = tester
CC = cc
CFLAGS = -Wall -Wextra -Werror

all: $(NAME)

$(NAME): tester.c
	$(CC) $(CFLAGS) tester.c -o $(NAME)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
