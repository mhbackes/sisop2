CC=gcc
CXXFLAGS=-g -Wall#-O3
BIN=bin
SRC=src
OBJ=$(BIN)/matrix-fork.o
TARGET=matrix-fork

$(TARGET): $(OBJ)
	$(CC) $(CXXFLAGS) -o $@ $^

$(BIN)/%.o: $(SRC)/%.c $(BIN)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -f $(BIN)/*.o *~ $(TARGET)

rebuild: clean $(TARGET)
