CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
INCLUDE = -I./include
LDFLAGS = -lm
CUNIT_LDFLAGS = -lcunit

SOURCES = src/main.c src/jogo.c 
OBJECTS = src/main.o src/jogo.o
EXECUTABLE = jogo

TEST_SOURCES = src/testar.c src/jogo.c
TEST_OBJECTS = src/testar.o src/jogo.o
TEST_EXECUTABLE = testar

.PHONY: all jogo testar clean

all: jogo testar

jogo: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(CFLAGS) $(LDFLAGS)

src/testar.o: src/testar.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

testar: $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $(TEST_EXECUTABLE) $(CFLAGS) $(LDFLAGS) $(CUNIT_LDFLAGS)
	./$(TEST_EXECUTABLE)
	
gcov: clean
	$(CC) -fprofile-arcs -ftest-coverage $(SOURCES) $(TEST_SOURCES) -o $(TEST_EXECUTABLE) $(LDFLAGS) $(CUNIT_LDFLAGS)
	./$(TEST_EXECUTABLE)
	gcov $(SOURCES) $(TEST_SOURCES)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(EXECUTABLE) $(TEST_EXECUTABLE) *.gcda *.gcno *.gcov src/*.o