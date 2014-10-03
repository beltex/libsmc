CC        = cc
CFLAGS     = -mmacosx-version-min=10.6 -std=c99 -arch x86_64 -O2 -Wall
FRAMEWORKS = -framework IOKit
SRC        = src/*.c
OBJ        = smc.o
LIB        = libsmc.a
LIB_DY     = libsmc.dylib

examples: static
	${CC} ${CFLAGS} ${FRAMEWORKS} -o ex_1.o examples/ex_1.c ${LIB}

examples_dy: dynamic
	${CC} ${CFLAGS} -o ex_1.o examples/ex_1.c ${LIB_DY}

static:
	${CC} ${CFLAGS} -c -o ${OBJ} ${SRC}
	libtool -static -o ${LIB} ${OBJ}

dynamic:
	${CC} ${CFLAGS} ${FRAMEWORKS} -dynamiclib -o ${LIB_DY} ${SRC}

clean: 
	rm *.o *.a *.dylib
