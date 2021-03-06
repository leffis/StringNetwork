CXX = g++   # compiler
CXXFLAGS = -g -Wall # compiler flags
MAKEFLAGS = --no-print-directory
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}   # makefile name

OBJECTS1 = client.o helper.o
DEPENDS1 = ${OBJECTS1:.o=.d}            # substitute ".o" with ".d"
EXEC1 = stringClient

OBJECTS2 = server.o helper.o
DEPENDS2 = ${OBJECTS2:.o=.d}            # substitute ".o" with ".d"
EXEC2 = stringServer

OBJECTS = ${OBJECTS1} ${OBJECTS2}               # all object files
DEPENDS = ${OBJECTS:.o=.d}                  # substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2}                   # all executables


all : ${EXECS}  
${EXEC1} : ${OBJECTS1}                      # link step 1st executable
	${CXX} ${CXXFLAGS} $^ -o $@ -lpthread


${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@


${OBJECTS} : ${MAKEFILE_NAME}                   # OPTIONAL : changes to this file => recompile

-include ${DEPENDS}                     # include *.d files containing program dependences

clean :                             # remove files that can be regenerated
	rm -f *.d *.o ${EXECS}