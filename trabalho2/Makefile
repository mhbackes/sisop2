CC=gcc
CXXFLAGS=-g -Wall#-O3
BIN=bin
SRC=src

all: philosopher-sem philosopher-mon

philosopher-sem: $(BIN)/philosopher-sem.o
	$(CC) $(CXXFLAGS) -o $@ $^

philosopher-mon: $(BIN)/philosopher-mon.o
	$(CC) $(CXXFLAGS) -o $@ $^

$(BIN)/%.o: $(SRC)/%.c | $(BIN)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)

.PHONY: rebuild
clean:
	rm -f $(BIN)/*.o *~ philosopher-sem philosopher-mon

.PHONY: rebuild
rebuild: clean all
