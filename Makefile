PROGS = main
OBJS = parse_json.o mem.o construct_packet.o

CFLAGS = -g

# Pattern rules to avoid explicit rules
%.o : %.c
	gcc ${CFLAGS} -c -o $@ $<

%.o : %.s
	as -o $@ $<

all : ${PROGS}

main: main.c parse_json.h parse_json.o mem.h mem.o construct_packet.h construct_packet.o logger.h
	gcc ${CFLAGS} -o $@ main.c parse_json.o mem.o construct_packet.o

clean :
	rm -rf ${PROGS} ${OBJS} *.dSYM
