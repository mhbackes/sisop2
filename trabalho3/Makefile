CC=gcc
FLAGS=-g -Wall#-O3
BIN=bin
SRC=src
CLIENT=client
SERVER=server
LIBS=-lpthread -lncurses

SRC_FILES=$(wildcard $(SRC)/*.c)
OBJ_FILES=$(patsubst $(SRC)/%.c, $(BIN)/%.o, $(SRC_FILES))
CLIENT_OBJ=$(filter-out $(BIN)/$(SERVER).o, $(OBJ_FILES))
SERVER_OBJ=$(filter-out $(BIN)/$(CLIENT).o, $(OBJ_FILES))

.PHONY: all
all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

$(SERVER): $(SERVER_OBJ)
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

$(BIN)/%.o: $(SRC)/%.c | $(BIN)
	$(CC) $(FLAGS) -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)

.PHONY: clean
clean:
	rm -f $(BIN)/*.o *~ *.dot $(CLIENT) $(SERVER) $(RM)

.PHONY: rebuild
rebuild: clean all
