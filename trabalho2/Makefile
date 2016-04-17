CC=gcc
CXXFLAGS=-g -std=c99 -Wall#-O3
BIN=bin
SRC=src
LIB=-lpthread

all: phil-sem phil-mon

phil-sem: $(BIN)/phil-sem.o
	$(CC) $(CXXFLAGS) -o $@ $^ $(LIB)

phil-mon: $(BIN)/phil-mon.o
	$(CC) $(CXXFLAGS) -o $@ $^ $(LIB)

$(BIN)/%.o: $(SRC)/%.c | $(BIN)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)

.PHONY: rebuild
clean:
	rm -f $(BIN)/*.o *~ phil-sem phil-mon

.PHONY: rebuild
rebuild: clean all
