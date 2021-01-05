.POSIX:
CC      = gcc
CFLAGS  = -Wall -Wextra -Os 
LDFLAGS =
LDLIBS  = -lm -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf  

obj = list.o main.o 

asteroid: $(obj)
	$(CC) $(LDFLAGS) -o $@ $(obj) $(LDLIBS)

clean:
	rm -f asteroid $(obj)
list.o: list.c list.h
main.o: main.c list.h
