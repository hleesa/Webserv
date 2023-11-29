NAME = webserv

CXX = c++

INCLUDE_DIR = ./inc/

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I $(INCLUDE_DIR)

SRCS_DIR = ./src/

CONFIG_DIR = $(SRCS_DIR)Config/
REQUEST_DIR = $(SRCS_DIR)Request/
RESPONSE_DIR = $(SRCS_DIR)Response/
SERVER_DIR = $(SRCS_DIR)Server/

CONFIG_SRC = \
			Config.cpp \
			ConfigParser.cpp \
			Location.cpp \
			type.cpp

REQUEST_SRC = \
			HttpRequestMessage.cpp \
			RequestParser.cpp

RESPONSE_SRC = \
			HttpResponseMessage.cpp \
			CodeToReason.cpp \
			Method.cpp \
			Get.cpp \
			GetCgi.cpp \
			Post.cpp \
			PostCgi.cpp\
			PostCgiPipePid.cpp\
			Delete.cpp \
			Head.cpp \
			Resource.cpp\
			ErrorPage.cpp \
			MediaType.cpp

SERVER_SRC = \
			Server.cpp \
			ServerManager.cpp \
			isVisibleString.cpp \
			serverUtils.cpp

FILES = main.cpp \

SRCS = \
	$(addprefix $(SRCS_DIR), $(FILES)) \
	$(addprefix $(CONFIG_DIR), $(CONFIG_SRC)) \
	$(addprefix $(REQUEST_DIR), $(REQUEST_SRC)) \
	$(addprefix $(RESPONSE_DIR), $(RESPONSE_SRC)) \
	$(addprefix $(SERVER_DIR), $(SERVER_SRC))

OBJS = $(SRCS:.cpp=.o)

RM = rm -rf

BLACK		= 	"\033[0;30m"
GRAY		= 	"\033[1;30m"
RED			=	"\033[1;31m"
GREEN		=	"\033[0;32m"
YELLOW		=	"\033[1;33m"
PURPLE		=	"\033[0;35m"
CYAN		=	"\033[1;36m"
WHITE		=	"\033[1;37m"
EOC			=	"\033[0;0m"
LINE_CLEAR	=	"\x1b[1A\x1b[M"

all: $(NAME)

%.o: %.cpp 
	@echo $(YELLOW) "Compiling...\t" $< $(EOC) $(LINE_CLEAR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@echo $(GREEN) "Source files are compiled!\n" $(EOC)
	@echo $(WHITE) "Building for" $(YELLOW) "$(NAME)" $(WHITE) "..." $(EOC)
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

clean:
	@$(RM) $(OBJS)
	@echo $(CYAN) "Clean...!"
	
fclean:
	@$(RM) $(OBJS) $(NAME)
	@echo $(RED) "Fclean...!"

re:
	@make fclean
	make all

.PHONY: all re clean fclean
