#Which compiler
cc = gcc

#Where are include files kept
INCLUDE = .

CFLAGS = -g -Wall -std=c99 -fPIC

ifeq ($(shell uname), Linux)
LINKFLAGS = -shared
else
LINKFLAGS = -undefined dynamic_lookup -dynamiclib
endif

ERLROOT = /opt/local/lib/erlang
#/usr/local/erl/lib/erlang/

objects = skiplist.o nif_skiplist.o
OUTPUT_NIF = nif_skiplist.so

all: $(objects)
	cc $(LINKFLAGS) -o $(OUTPUT_NIF) $(objects)

skiplist.o: skiplist.h skiplist.c
	$(cc) $(CFLAGS) -c skiplist.c
nif_skiplist.o: skiplist.h nif_skiplist.c
	$(cc) $(CFLAGS) -I $(ERLROOT)/usr/include/ -c nif_skiplist.c

# 伪目标
.PHONY: clean

# 加减号的意思是，也许某些文件出现问题，但不要管，继续做后面的事
clean:
	-rm -f $(objects) $(OUTPUT_NIF)