CC     := gcc
CFLAGS := -Wall -Werror -I../include -pthread

OS     := $(shell uname -s)
LIBS   := 
ifeq ($(OS),Linux)
	LIBS += -pthread
endif

SRCS   := hashdb.c \
	chash.c \
	rwlock.c

OBJS   := ${SRCS:c=o}
PROGS  := ${SRCS:.c=}

.PHONY: all
all: ${PROGS}

${PROGS} : % : %.o Makefile
	${CC} $< -o $@ ${LIBS}

clean:
	rm -f ${PROGS} ${OBJS}

%.o: %.c Makefile
	${CC} ${CFLAGS} -c $<