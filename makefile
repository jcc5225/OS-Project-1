TARGET 	= yash

CC		= gcc
CFLAGS 	= -Wall -g -I inc/
LIBS	= -lreadline 

LINKER 	= gcc
LFLAGS 	= -Wall -g

SRCDIR 	= ./src
OBJDIR  = ./obj
INCDIR  = ./inc

all : 
	$(CC) $(CFLAGS) -o yash $(SRCDIR)/*.c $(LIBS)

clean:
	rm -f obj/*.o
	rm -f yash
