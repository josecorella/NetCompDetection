PROGS = main
OBJS = parse_json.o

CFLAGS = -g

# Pattern rules to avoid explicit rules
%.o : %.c
	gcc ${CFLAGS} -c -o $@ $<

%.o : %.s
	as -o $@ $<

all : ${PROGS}

main: main.c parse_json.h parse_json.o
	gcc ${CFLAGS} -o $@ main.c parse_json.o

clean :
	rm -rf ${PROGS} ${OBJS} *.dSYM
