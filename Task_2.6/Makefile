CC=g++
CFLAGS=-c -Werror -pedantic-errors -Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wextra-semi -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -Wduplicated-branches -Wduplicated-cond -Wconversion -Wunreachable-code -Wnarrowing -Wredundant-decls -Wlogical-op
LDFLAGS=-fsanitize=address
SOURCES=main.cpp source.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Monte_Carlo

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LDFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

source.o: source.cpp
	$(CC) $(CFLAGS) source.cpp

clean:
	rm -rf *.o Monte_Carlo
