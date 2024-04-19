CC = gcc
CFLAGS = -Wall -pthread
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
EXEC = chash

# Rule to compile .o files from .c files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

.PHONY: clean

clean:
	rm -f *.o $(EXEC)
