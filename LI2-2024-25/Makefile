CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
LDFLAGS = -fsanitize=address
LIBS = -lcunit

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include
TEST_DIR = test

# Arquivos fonte
SRCS = $(SRC_DIR)/jogo.c $(SRC_DIR)/main.c
TEST_SRCS = $(TEST_DIR)/test_jogo.c $(SRC_DIR)/jogo.c

# Objetos correspondentes
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TEST_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(filter $(SRC_DIR)/%.c, $(TEST_SRCS)))
TEST_OBJS += $(OBJ_DIR)/test_jogo.o

# Executáveis
JOGO_EXEC = $(BIN_DIR)/jogo
TEST_EXEC = $(BIN_DIR)/testar

# Regras
all: jogo testar

jogo: $(JOGO_EXEC)

testar: $(TEST_EXEC)
	$(TEST_EXEC)

coverage: $(TEST_EXEC)
	mkdir -p coverage
	gcov -o $(OBJ_DIR) $(SRC_DIR)/*.c
	mv *.gcov coverage/

$(JOGO_EXEC): $(OBJS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(TEST_EXEC): $(TEST_OBJS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

$(OBJ_DIR)/test_jogo.o: $(TEST_DIR)/test_jogo.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/* coverage/*.gcov

.PHONY: all jogo testar clean coverage