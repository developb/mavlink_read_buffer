

CFLAGS= -g -w
CC=gcc
#LD=ld

INCLUDE := .

SRL_SRC := srl_io.c main_rx.c rt.c
SRL_OBJ := $(SRL_SRC:.c=.o)

%.o : %.c
	$(CC) $(CFLAGS) -I$(INCLUDE) $< -c -o $@

all: mav_read mav_decode

mav_read: $(SRL_OBJ)
	$(CC) $(SRL_OBJ) -o $@ -lrt -lc -lpthread

mav_decode:
	$(CC) read_binaryfile.c -o $@ -lc

clean:
	-rm -f mav_decode mav_read *.o
