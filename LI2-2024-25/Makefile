CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g --coverage
INCLUDE = -I./include
LDFLAGS = -lm --coverage
CUNIT_LDFLAGS = -lcunit

SRC_DIR = src
OBJ_DIR = obj

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/jogo.c
OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/jogo.o
EXECUTABLE = jogo

TEST_SOURCES = $(SRC_DIR)/testar.c $(SRC_DIR)/jogo.c
TEST_OBJECTS = $(OBJ_DIR)/testar.o $(OBJ_DIR)/jogo.o
TEST_EXECUTABLE = testar

.PHONY: all jogo clean test coverage

all: jogo

jogo: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(CFLAGS) $(LDFLAGS)
	./$(EXECUTABLE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(EXECUTABLE) $(TEST_EXECUTABLE) *.gcda *.gcno *.gcov
	rm -rf $(OBJ_DIR)

testar: $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $(TEST_EXECUTABLE) $(CFLAGS) $(LDFLAGS) $(CUNIT_LDFLAGS)
	./$(TEST_EXECUTABLE)

coverage: clean testar
	gcov -o $(OBJ_DIR) $(SRC_DIR)/jogo.c $(SRC_DIR)/testar.c
