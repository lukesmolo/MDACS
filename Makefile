CC      =g++
DEBUG   =-Wall -g -pedantic
OTHERS  =-Wl,--export-dynamic -Weffc++
INCLUDE =-I../
CFLAGS= -c -ansi  -std=c++0x  -g -I./include
THREADS =-lpthread  -D_REENTRANT
LIBSLINUX=  -L./libLinux -lEngine -lGL -lglut -lm -lGLU
LDFLAGS= -lm -g -lstdc++

SOURCES_SERVER=$(filter-out src/Client.cpp, $(wildcard src/*.cpp ))
SOURCES_CLIENT =src/Client.cpp
HEADERS= $(wildcard src/*.h src/*/*.h src/*/*/*.h)
OBJS_SERVER = ${SOURCES_SERVER:.cpp=.o}
OBJS_CLIENT = ${SOURCES_CLIENT:.cpp=.o}

EXE_SERVER=main
EXE_CLIENT=client
MAIN = Server.o
CLIENT = Client.o

all:	${OBJS_SERVER} ${OBJS_CLIENT}
	${CC}  ${OBJS_SERVER} -o ${EXE_SERVER} ${LDFLAGS} ${LIBSLINUX}  ${THREADS}
	${CC}  ${OBJS_CLIENT} -o ${EXE_CLIENT} ${LDFLAGS} ${LIBSLINUX}  ${THREADS}

%.o:%.cpp
	${CC} -o $@ -c $< ${INCLUDE} $(CFLAGS) ${THREADS}

clean:
	rm -fv *.o src/*.o src/*/*.o src/*/*/*.o main client


run:

	urxvt -e ./client &
	urxvt -e ./client &
	urxvt -e ./client &
	urxvt -e ./client &
	urxvt -e ./client &
	./main



