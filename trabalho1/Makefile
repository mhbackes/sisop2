CC=gcc
CXXFLAGS=-g -Wall#-O3
BIN=bin
SRC=src
OBJ=$(BIN)/matrix.o $(BIN)/matrix-thread.o $(BIN)/matrix-fork.o 

all: matrix-thread matrix-fork

matrix-thread: $(BIN)/matrix.o $(BIN)/matrix-thread.o
	$(CC) $(CXXFLAGS) -o $@ $^ -lpthread

matrix-fork: $(BIN)/matrix.o $(BIN)/matrix-fork.o
	$(CC) $(CXXFLAGS) -o $@ $^ -lpthread

$(BIN)/%.o: $(SRC)/%.c $(BIN)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -f $(BIN)/*.o *~ matrix-thread matrix-fork

rebuild: clean all
