NAME	=	game

GCC 	= 	gcc

CFLAGS 	= 	-std=c11 	\
			-Wall		\
			-pedantic	\
			-ggdb		\

CLIBS	=	`pkg-config --cflags --libs allegro-5.0 allegro_dialog-5.0 allegro_font-5.0 allegro_ttf-5.0 allegro_primitives-5.0 allegro_image-5.0 glib-2.0` \
			-lallegro_tiled \
			-lz \
			-lxml2 \
			-lm \

SRC		=	main.c	\
			player.c\

OBJ		= 	${SRC:.c=.o}

${NAME}:	${OBJ}
			${GCC} ${CLAGS} ${OBJ} ${CLIBS} -o ${NAME}

all:		${NAME}

clean:
			rm -vf ${OBJ}

fclean:		clean
			rm -vf ${NAME}

re:			fclean all
