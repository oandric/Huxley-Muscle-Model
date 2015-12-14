NAME		= HuxleyMuscleModel
EXEC		= $(NAME)

CC		= gcc
CFLAGS		= -std=gnu99 -Wall
LDFLAGS		= -lm

OBJ		= HuxleyMuscleModel.o

all:		run

$(EXEC):	$(OBJ)
		$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

run:		$(EXEC)

.INTERMEDIATE:	$(OBJ)

clean:
		rm $(EXEC)	
