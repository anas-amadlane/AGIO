# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/14 10:59:59 by aamadlan          #+#    #+#              #
#    Updated: 2023/03/26 21:23:06 by aamadlan         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	libft.a

CFLAGS	=	-Wall -Wextra -Werror

SRC		=	ft_isdigit.c\
			ft_putnbr_fd.c\
			ft_strlcpy.c\
			ft_tolower.c\
			ft_isprint.c\
			ft_memchr.c\
			ft_putstr_fd.c\
			ft_strlen.c\
			ft_toupper.c\
			ft_atoi.c\
			ft_itoa.c\
			ft_memcmp.c\
			ft_split.c\
			ft_strmapi.c\
			ft_bzero.c\
			ft_memcpy.c\
			ft_strchr.c\
			ft_strncmp.c\
			ft_calloc.c\
			ft_memmove.c\
			ft_strdup.c\
			ft_strnstr.c\
			ft_isalnum.c\
			ft_memset.c\
			ft_striteri.c\
			ft_strrchr.c\
			ft_isalpha.c\
			ft_putchar_fd.c\
			ft_strjoin.c\
			ft_strtrim.c\
			ft_isascii.c\
			ft_putendl_fd.c\
			ft_strlcat.c\
			ft_substr.c\
			ft_lstnew_bonus.c\
			ft_lstadd_front_bonus.c\
			ft_lstsize_bonus.c\
			ft_lstlast_bonus.c\
			ft_lstadd_back_bonus.c\
			ft_lstdelone_bonus.c\
			ft_lstclear_bonus.c\
			ft_lstiter_bonus.c\
			ft_lstmap_bonus.c\
			get_next_line_bonus.c\
			ft_printf.c\
			ft_printf_utils.c

OBJ		=	$(SRC:.c=.o)

all		:	$(NAME)

$(NAME)	:	$(OBJ)
		@ ar -rc $(NAME) $(OBJ)

$(OBJ)	:	%.o : %.c
		@ $(CC) $(CFLAGS) -c $< -o $@

clean	:
		@ rm -f $(OBJ)

fclean	:	clean
		@ rm -f $(NAME)

re		:	fclean all

.PHONY	:	all clean fclean re

# BNS_SRC = ft_lstnew_bonus.c ft_lstadd_front_bonus.c ft_lstsize_bonus.c ft_lstlast_bonus.c ft_lstadd_back_bonus.c ft_lstdelone_bonus.c ft_lstclear_bonus.c ft_lstiter_bonus.c ft_lstmap_bonus.c
# BNS_OBJ = $(BNS_SRC:.c=.o)
# bonus: all $(BNS_OBJ)
# 	ar -r $(NAME) $(BNS_OBJ)