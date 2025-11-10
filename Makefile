# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rubengallien <rubengallien@student.42.f    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/28 16:25:07 by lvicino           #+#    #+#              #
#    Updated: 2025/11/09 22:15:11 by rubengallie      ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	webserv

BUILD_DIR	=	build



CONF_LIB	=	src/conf/conf.a
MAKE_CONF	=	make -C src/conf

CONF_SRC	=	$(wildcard src/conf/src/*.cpp) #need to fix
CONF_OBJ	=	$(CONF_SRC:src/conf/src/%.cpp=src/conf/build/%.o)
CONF_DEP	=	$(CONF_OBJ:%.o=%.d)


SERVER_LIB	=	src/server/server.a
MAKE_SERVER	=	make -C src/server

SERVER_SRC	=	$(wildcard src/server/src/*.cpp) #need to fix
SERVER_OBJ	=	$(SERVER_SRC:src/server/src/%.cpp=src/server/build/%.o)
SERVER_DEP	=	$(SERVER_OBJ:%.o=%.d)


CGI_LIB	=	src/cgi/cgi.a
MAKE_CGI	=	make -C src/cgi

CGI_SRC	=	$(wildcard src/cgi/src/*.cpp) #need to fix
CGI_OBJ	=	$(CGI_SRC:src/cgi/src/%.cpp=src/cgi/build/%.o)
CGI_DEP	=	$(CGI_OBJ:%.o=%.d)



SRC_DIR		=	src

MAIN		=	main.cpp

MAIN		:=	$(MAIN:%=$(SRC_DIR)/%)
MAIN_OBJ	=	$(MAIN:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
MAIN_DEP	=	$(MAIN_OBJ:$(BUILD_DIR)/%.o=$(BUILD_DIR)/%.d)



CC			=	c++
CFLAGS		=	-Wall -Werror -Wextra -MMD -MP -std=c++98
INCLUDE		=	-I src/conf/include \
				-I src/server/include \
				-I src/server/include/Utils \
				-I src/cgi/include

MAKEFLAGS	+=	--no-print-directory
DIR_DUP		=	mkdir -p $(@D)


all		:	$(NAME)

$(NAME)	:	$(MAIN_OBJ) $(CONF_LIB) $(SERVER_LIB) $(CGI_LIB)
	$(CC) $(MAIN_OBJ) $(CONF_LIB) $(SERVER_LIB) $(CGI_LIB) -o $(NAME)

$(BUILD_DIR)/%.o	:	$(SRC_DIR)/%.cpp
	$(DIR_DUP)
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<


$(CONF_LIB)	:	$(CONF_SRC)
	$(MAKE_CONF)

$(SERVER_LIB)	:	$(SERVER_SRC)
	$(MAKE_SERVER)

$(CGI_LIB)	:	$(CGI_SRC)
	$(MAKE_CGI)


-include $(MAIN_DEP) $(CONF_DEP) $(SERVER_DEP) $(CGI_DEP)

clean	:
	if [ -d "$(BUILD_DIR)" ]; then \
		rm -rf $(BUILD_DIR); \
	fi
	$(MAKE_CONF) clean
	$(MAKE_SERVER) clean
	$(MAKE_CGI) clean

fclean	:	clean
	if [ -e "$(NAME)" ]; then \
		rm -f $(NAME); \
	fi
	$(MAKE_CONF) fclean
	$(MAKE_SERVER) fclean
	$(MAKE_CGI) fclean

re		:	fclean all

.PHONY	:	clean fclean re
.SILENT	:	clean fclean
