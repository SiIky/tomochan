EXEC=tomochan

CC=cc
SRC=tomochan.c
OBJS=$(SRC:.c=.o)
CFLAGS=-std=c11 -Wall -Wextra -Wconversion -pedantic -flto -lcurl -O3

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)
	strip -s $(EXEC)

clean:
	rm -rf $(OBJS) $(EXEC)
