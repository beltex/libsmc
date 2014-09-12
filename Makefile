CC        = cc
CFLAGS    = -mmacosx-version-min=10.6 -std=c99 -arch x86_64 -O2 -Wall
LIB_FLAGS = -framework IOKit
SRC       = src/*.c
OBJ       = smc.o
LIB       = libc-smc.a

examples: static
	${CC} ${CFLAGS} ${LIB_FLAGS} -o ex_1.o examples/ex_1.c ${LIB}

static:
	${CC} ${CFLAGS} -c -o ${OBJ} ${SRC}
	libtool -static -o ${LIB} ${OBJ}

clean: 
	rm *.o *.a
