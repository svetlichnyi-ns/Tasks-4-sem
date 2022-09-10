CC=g++
CFLAGS=-Werror -pedantic-errors -Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wextra-semi -Wshadow -Wpointer-arith
CFLAGS+=-Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -Wduplicated-branches -Wduplicated-cond
CFLAGS+=-Wconversion -Wunreachable-code -Wnarrowing -Wredundant-decls -Wlogical-op -flto -fsanitize=address,undefined
SOURCES=main.cpp source.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Monte_Carlo

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS)

$(OBJECTS): $(SOURCES)
	$(CC) -c $(CFLAGS) $(SOURCES)

clean:
	rm $(OBJECTS) $(EXECUTABLE)