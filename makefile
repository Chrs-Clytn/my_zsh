NAME = my_zsh
SRC = main.c my_zsh.c
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g3 -fsanitize=address

OBJ = obj/main.o\
	obj/my_zsh.o\

CFLAGS += -Iinclude -Wall -Wextra -Werror -g3 -fsanitize=address

HEADER = include/main.h\

BIN = bin/my_zsh
CC = gcc
RM = rm -rf

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(CFLAGS)

obj/main.o: src/main.c
	${CC} ${CFLAGS} -c $< -o $@
obj/my_zsh.o: src/my_zsh.c
	${CC} ${CFLAGS} -c $< -o $@

clean:
	$(RM) bin/my_zsh bin/*.dSYM obj/*