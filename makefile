OBJS	= cmdline.o shell.o
SOURCE	= cmdline.c shell.c
HEADER	= 
OUT	= shell
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

cmdline.o: cmdline.c
	$(CC) $(FLAGS) cmdline.c 

shell.o: shell.c
	$(CC) $(FLAGS) shell.c 


clean:
	rm -f $(OBJS) $(OUT)