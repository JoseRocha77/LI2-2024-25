CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g --coverage
INCLUDE = -I./include
LDFLAGS = -lm --coverage
CUNIT_LDFLAGS = -lcunit

SOURCES = src/main.c src/jogo.c
OBJECTS = src/main.o src/jogo.o
EXECUTABLE = jogo

TEST_SOURCES = src/testar.c src/jogo.c
TEST_OBJECTS = src/testar.o src/jogo.o
TEST_EXECUTABLE = testar

.PHONY: all jogo clean test coverage

all: jogo

jogo: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(CFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(TEST_EXECUTABLE) *.gcda *.gcno *.gcov src/*.o src/*.gcda src/*.gcno src/*.gcov

# --- Testes unitários com gcov ---
testar: $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $(TEST_EXECUTABLE) $(CFLAGS) $(LDFLAGS) $(CUNIT_LDFLAGS)
	./$(TEST_EXECUTABLE)

coverage: clean testar
	gcov src/*.c