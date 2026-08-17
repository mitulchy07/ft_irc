NAME = ircserv
SRCS = src/main.cpp \
       src/client/Client.cpp \
       src/server/Server.cpp \
       src/server/ServerClient.cpp \
       src/server/ServerInit.cpp \
       src/server/ServerLoop.cpp \
       src/server/ServerSocket.cpp \
       src/parser/Parser.cpp \
	   src/channel/Channel.cpp \
       src/commands/Command.cpp

OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.o))

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# Colors
RED = \e[0;31m
GREEN = \e[0;32m
YELLOW = \e[0;33m
BLUE = \e[0;34m
MAGENTA = \e[0;35m
CYAN = \e[0;36m
NC = \e[0m

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(BLUE)[LINK]$(NC) $(NAME)"
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)[CC]$(NC) $<"
	@$(CXX) $(CXXFLAGS) -Iinclude -c $< -o $@

clean:
	@echo "$(RED)[CLEAN]$(NC) removing objects"
	@rm -rf $(OBJDIR)

fclean: clean
	@echo "$(RED)[FCLEAN]$(NC) removing $(NAME)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re