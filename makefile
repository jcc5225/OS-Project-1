TARGET 	= yash

CC		= gcc
CFLAGS 	= -Wall -g -c -I inc/
LIBS	= -lreadline 

LINKER 	= gcc
LFLAGS 	= -Wall -g

SRCDIR 	= ./src
OBJDIR  = ./obj
INCDIR  = ./inc

$(TARGET) : yash.o 
	@$(LINKER) $(LFLAGS) -o $@ $(OBJDIR)/*.o $(LIBS)
yash.o : $(SRCDIR)/yash.c $(INCDIR)/yash.h parse.o command.o
	@$(CC) $(CFLAGS) $< -o $(OBJDIR)/$@ $(LIBS)
parse.o : $(SRCDIR)/parse.c $(INCDIR)/parse.h
	@$(CC) $(CFLAGS) $< -o $(OBJDIR)/$@ $(LIBS)
command.o : $(SRCDIR)/command.c $(INCDIR)/command.h
	@$(CC) $(CFLAGS) $< -o $(OBJDIR)/$@ $(LIBS)

clean:
	rm -f obj/*.o
	rm -f yash
