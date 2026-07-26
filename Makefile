NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -Iinclude

SRCS = src/main.cpp \
       src/client/Client.cpp \
       src/server/Server.cpp \
       src/server/ServerClient.cpp \
       src/server/ServerInit.cpp \
       src/server/ServerLoop.cpp \
       src/server/ServerSocket.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
