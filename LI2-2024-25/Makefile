CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
LDFLAGS = -lm

SOURCES = src/main.c src/jogo.c 
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = jogo

TEST_SOURCES = src/testar.c src/jogo.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
TEST_EXECUTABLE = testar

.PHONY: all jogo testar clean

all: jogo testar

jogo: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(CFLAGS) $(LDFLAGS)

testar: $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $(TEST_EXECUTABLE) $(CFLAGS) $(LDFLAGS)
	./$(TEST_EXECUTABLE)
	
gcov: clean
	$(CC) -fprofile-arcs -ftest-coverage $(TEST_SOURCES) -o $(TEST_EXECUTABLE) $(LDFLAGS)
	./$(TEST_EXECUTABLE)
	gcov $(SOURCES)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(EXECUTABLE) $(TEST_EXECUTABLE) *.gcda *.gcno *.gcov