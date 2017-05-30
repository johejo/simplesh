CC = gcc
PROGRAM=simplesh
CFLAGS = -Wall
OBJS = main.o
$(PROGRAM) : $(OBJS)
	$(CC) $(OBJS) -o $(PROGRAM)
main.o : main.c defs.h
	$(CC) -c $(CFLAGS) main.c

clean:
	-rm -rf $(OBJS) $(PROGRAM)