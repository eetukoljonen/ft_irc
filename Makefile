NAME := ircserv
CC := c++
COMPILE_FLAGS := -Wall -Wextra -Werror -std=c++11
SRC := main.cpp Server.cpp User.cpp CommandExecution.cpp Command.cpp Utils.cpp Channel.cpp
OBJECT = $(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))
OBJDIR = obj
SRCDIR = src


.PHONY: all clean fclean re

all: obj $(OBJDIR) $(NAME)

obj:
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(COMPILE_FLAGS) -c $< -o $@

$(NAME): $(OBJECT)
	$(CC) $(OBJECT) -o $(NAME)

$(OBJ): $(SRC)
	$(CC) -c $(COMPILE_FLAGS) $(SRC)

clean:
	rm -rf $(OBJDIR)
	rm -f $(OBJECT)

fclean: clean
	rm -f $(NAME)

re: fclean all
