CC=gcc
CXXFLAGS=-g -Wall#-O3
BIN=bin
SRC=src
OBJ=$(BIN)/matrix-thread.o $(BIN)/matrix.o
TARGET=matrix-thread

$(TARGET): $(OBJ)
	$(CC) $(CXXFLAGS) -o $@ $^ -lpthread

$(BIN)/%.o: $(SRC)/%.c $(BIN)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -f $(BIN)/*.o *~ $(TARGET)

rebuild: clean $(TARGET)
